#include <iostream>
#include <vector>
#include <cmath>
#include <ilcplex/ilocplex.h>
#include "ILPStrategy.h"

using namespace std;

struct MigrationRequest 
{
    int Request_ID;
    int RequestType;
    int t_start;
    int duration;
    int core_count_bucket;
    double nCPU;
    int FPGA;
    int ram_bucket;
    int Disk;
    int BW;
};

namespace AllocationStrategies
{

ILP::ILP()
{
    m_migrationCost = 250;
    m_maximumRequestsInPM = 1e5;
    m_Tau = 0.75;
}

ILP::~ILP()
{
}

std::vector<AllocationResult> ILP::Run(std::vector<PhysicalMachine> &machines, std::vector<EventTypes::Request> &requests)
{
    std::vector<AllocationResult> allocationResults;

    int I = machines.size();
    int J = requests.size();
    std::vector<MigrationRequest> stpMigRequestArray{};
    int nMig = stpMigRequestArray.size();

    // Matrices initialization for resource constraint
    vector<vector<int>> d_pj(J, vector<int>(I, 0));
    vector<vector<int>> A1(I, vector<int>(J, 0)); // CPU Constraints
    vector<vector<int>> A2(I, vector<int>(J, 0)); // Memory Constraints
    vector<vector<int>> A3(I, vector<int>(J, 0)); // Disk Constraints
    vector<vector<int>> A4(I, vector<int>(J, 0)); // Bandwidth Constraints
    vector<vector<int>> A5(I, vector<int>(J, 0)); // FPGA Constraints

    // Fill matrices with data
    for (int j = 0; j < J; ++j) 
    {
        for (int i = 0; i < I; ++i) 
        {
            // Ensuring the assignment constraints
            d_pj[j][i] = 1;

            // Ensuring CPU constraints
            A1[i][j] = machines[i].GetAvailableResources().CoreCount;

            // Ensuring Memory constraints
            A2[i][j] = machines[i].GetAvailableResources().RAM;

            // Ensuring Disk constraints
            A3[i][j] = machines[i].GetAvailableResources().Disk;

            // Ensuring Bandwidth constraints
            A4[i][j] = machines[i].GetAvailableResources().Bandwidth;

            // Ensuring FPGA constraints
            A5[i][j] = machines[i].GetAvailableResources().FPGA;
        }
    }

    // CPLEX model
    IloEnv env;
    try 
    {
        IloModel model(env);

        // Decision variables

        IloArray<IloBoolVarArray> x_newcomers(env, J);  // Newcomer Requests to assign
        for (int j = 0; j < J; ++j) 
        {
            x_newcomers[j] = IloBoolVarArray(env, I);
        }

        IloArray<IloBoolVarArray> x_migrations(env, nMig); // Migration Requests to assign
        for (int j = 0; j < nMig; ++j) 
        {
            x_migrations[j] = IloBoolVarArray(env, I);
        }
        
        IloBoolVarArray y(env, I);   // PM activation status
        for (int i = 0; i < I; ++i) 
        {
            y[i] = IloBoolVar(env);  // PM is turned on or not
        }

        IloBoolVarArray migrate(env, nMig); // Indicates if migration request is actually migrated
        for (int j = 0; j < nMig; ++j) 
        {
            for (int i = 0; i < I; ++i) {
                x_migrations[j][i] = IloBoolVar(env);
            }
            migrate[j] = IloBoolVar(env);
        }

        // Calculation of CpuUtilPer for eaxh PM for Cost 3

        // COST FUNCTION
        
        IloExpr cost(env);
        // Cost 1: Turning on a PM cost
        for (int i = 0; i < I; ++i) 
        {
            cost += y[i] * (machines[i].IsPoweredOn() ? 1 : 100);
        }
      
        // Cost 2: Adding migration costs
        for (int j = 0; j < nMig; ++j) 
        {
            cost += migrate[j] * m_migrationCost;  // Additional cost for each migration
        }

        // Cost 3: Adding a dynamic cost depends on to PMs utilization for newcomer requests
        for (int i = 0; i < I; ++i) 
        {
            // Calculate the CPU utilization dynamically (assuming current assignments are part of the model)
            double nCPUUtilization = floor(((machines[i].GetAvailableResources().CoreCount * -1.0) / machines[i].GetResourceSpecs().CoreCount) * 100.0 + 100);

            for (int j = 0; j < J; ++j) 
            {
                double additionalCost = 0;
                if (nCPUUtilization < 45) 
                {
                    additionalCost = machines[i].GetPowerSpecs().CPU * (300 - 4 * nCPUUtilization) * requests[j].m_RequestedResources.CoreCount;
                }
                else 
                {
                    additionalCost = machines[i].GetPowerSpecs().CPU * (4 * nCPUUtilization - 60) * requests[j].m_RequestedResources.CoreCount;
                }
                cost += x_newcomers[j][i] * additionalCost;
            }
        }

        // Cost 4: Adding a dynamic cost depends on to PMs utilization for migration requests
        for (int i = 0; i < I; ++i) 
        {
            // Calculate the CPU utilization dynamically (assuming current assignments are part of the model)
            double nCPUUtilization = floor(((machines[i].GetAvailableResources().CoreCount * -1.0) / machines[i].GetResourceSpecs().CoreCount) * 100.0 + 100);

            for (int j = 0; j < nMig; ++j) // NOTE: Changed here (J to nMig)
            {
                double additionalCost = 0;
                if (nCPUUtilization < 45) 
                {
                    additionalCost = machines[i].GetPowerSpecs().CPU * (300 - 4 * nCPUUtilization) * stpMigRequestArray[j].core_count_bucket;
                }
                else 
                {
                    additionalCost = machines[i].GetPowerSpecs().CPU * (4 * nCPUUtilization - 60) * stpMigRequestArray[j].core_count_bucket;
                }
                cost += x_migrations[j][i] * additionalCost;
            }
        }

        model.add(IloMinimize(env, cost));

        //CONSTRAINTS

        // Constraint 1: Each request can only be assigned to one PM
        for (int j = 0; j < J; ++j) 
        {
            IloExpr sum(env);
            for (int i = 0; i < I; ++i) 
            {
                sum += x_newcomers[j][i];
            }
            //model.add(sum == 1); // NOTE: Changed here (== to <=)
            model.add(IloRange(env, 1, sum, 1, "AllocatedToOnePM"));
            sum.end();
        }

        // Constraint 2: Resource constraints using A1, A2, A3, A4, A5 matrices
        for (int i = 0; i < I; ++i) 
        {
            IloExpr sumCPU(env);
            IloExpr sumFPGA(env);
            IloExpr sumMemory(env);
            IloExpr sumDisk(env);
            IloExpr sumBW(env);

            for (int j = 0; j < J; ++j) 
            {
                sumCPU += x_newcomers[j][i] * static_cast<int>(requests[j].m_RequestedResources.CoreCount);
                sumFPGA += x_newcomers[j][i] * static_cast<int>(requests[j].m_RequestedResources.FPGA);
                sumMemory += x_newcomers[j][i] * requests[j].m_RequestedResources.RAM;
                sumDisk += x_newcomers[j][i] * requests[j].m_RequestedResources.Disk;
                sumBW += x_newcomers[j][i] * requests[j].m_RequestedResources.Bandwidth;
            }

            for (int k = 0; k < nMig; ++k) 
            {
                sumCPU += x_migrations[k][i] * ceil(stpMigRequestArray[k].nCPU);
                sumFPGA += x_migrations[k][i] * stpMigRequestArray[k].FPGA;
                sumMemory += x_migrations[k][i] * stpMigRequestArray[k].ram_bucket;
                sumDisk += x_migrations[k][i] * stpMigRequestArray[k].Disk;
                sumBW += x_migrations[k][i] * stpMigRequestArray[k].BW;
            }

            model.add(sumCPU <= A1[i][0]);
            // cout << "sumCPU: " << sumCPU << "and A1[" << i << "][0] is " << A1[i][0] <<endl;
            model.add(sumMemory <= A2[i][0]);
            model.add(sumDisk <= A3[i][0]);
            model.add(sumBW <= A4[i][0]);
            model.add(sumFPGA <= A5[i][0]);

            sumCPU.end();
            sumMemory.end();
            sumDisk.end();
            sumBW.end();
            sumFPGA.end();
        }

        // Constraint 3: Link PM activation to request assignment using the Big-M method
        for (int i = 0; i < I; ++i) 
        {
            IloExpr sum(env);
            // Sum up assignment variables for newcomer requests
            for (int j = 0; j < J; ++j) 
            {
                sum += x_newcomers[j][i];
            }
            // Sum up assignment variables for migration requests
            for (int j = 0; j < nMig; ++j) 
            {
                sum += x_migrations[j][i];
            }
            // Ensure the PM is turned on if it has any requests assigned to it
            model.add(sum <= m_maximumRequestsInPM * y[i]);  // If any request is assigned, PM must be on
            sum.end();
        }

        // Constraint 4: for each migration request should be migrated to 1 PM or not migrate
        for (int j = 0; j < nMig; ++j) 
        {
            IloExpr sumMigrationAssignments(env);
            for (int i = 0; i < I; ++i) 
            {
                sumMigrationAssignments += x_migrations[j][i];
            }
            model.add(sumMigrationAssignments == migrate[j]); // Linking migration decision to the assignment of PMs
            sumMigrationAssignments.end();
        }

        // Constraint 5: It is the constraint about how much load we need to migrate according to the TAM value defined in my thesis
        IloExpr remainingCPU(env);
        for (int j = 0; j < nMig; ++j) 
        {
            remainingCPU += (1 - migrate[j]) * ceil(stpMigRequestArray[j].nCPU);  // CPU load of non-migrated requests
        }
        int totalCPUCapacity = machines[0].GetResourceSpecs().CoreCount;  // Assuming PMq is defined and nTotalCPU is its total CPU capacity
        // TODO: violatedPM
    
        //cout << "*********The CPU target after Allcation is:  " << Tau * stpPM_Array[violatedPM].nTotalCPU << endl;
        model.add(remainingCPU <= m_Tau * totalCPUCapacity);  // Constraint to keep remaining load within threshold
        remainingCPU.end();


        //  SOLVE THE ILP
            
        IloCplex cplex(model);
        cplex.setOut(env.getNullStream());
        cplex.solve();

        // Output results
        cout << "Objective Value: " << cplex.getObjValue() << endl;
        for (int j = 0; j < J; ++j) 
        {
            for (int i = 0; i < I; ++i) 
            {
                if (cplex.getValue(x_newcomers[j][i]) > 0.5) 
                {
                    cout << "NewcomerRequest " << j << " is assigned to PM " << i << endl;

                    AllocationResult result;
                    result.physicalMachine = &machines.at(i);
                    result.request = &requests.at(j);
                    allocationResults.push_back(result);
                }
            }
        }

        cout << "Migration Decisions:" << endl;
        for (int j = 0; j < nMig; ++j) 
        {
            if (cplex.getValue(migrate[j]) > 0.5) 
            {
                cout << "MigrationRequest " << j << " is migrated. Assignments: ";
                for (int i = 0; i < I; ++i) 
                {
                    if (cplex.getValue(x_migrations[j][i]) > 0.5) 
                    {
                        cout << "PM " << i << " ";
                    }
                }
                cout << endl;
            }
            else 
            {
                cout << "MigrationRequest " << j << " is not migrated." << endl;
            }
        }

        cplex.clear();
    }
    catch (IloException& e) 
    {
        cerr << "Exception: " << e << endl;
    }

    env.end();

    return allocationResults;
}

void ILP::SetMigrationCost(int cost)
{
    m_migrationCost = cost;
}

void ILP::SetTau(double tau)
{
    m_Tau = tau;
}

}