// Markus Buchholz

#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <math.h>
#include <map>
#include <algorithm>
#include <numeric>


auto INF = std::numeric_limits<float>::max();

float xmin = 0.05;
float xmax = 2.0;
float ymin = 0.25;
float ymax = 1.3;
float zmin = 2.0;
float zmax = 15.0;

//--------------------------------------------------------------------------------
int BIRDS = 30;
int EVOLUTIONS = 1000;
int MOD = 2 * BIRDS;
float A = 2.0;
float R = 0.045;

//--------------------------------------------------------------------------------

struct Pos
{

    float x;
    float y;
    float z;
};

//--------------------------------------------------------------------------------

float generateRandom()
{

    std::random_device engine;
    std::mt19937 generator(engine());
    std::uniform_real_distribution<float> distrib(0.0, 1.0);
    return distrib(generator);
}

//--------------------------------------------------------------------------------
int generateRandomPerm()
{

    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<int> distribution(0, 2);
    return distribution(generator);
}

//--------------------------------------------------------------------------------

float valueGenerator(float low, float high)
{

    return low + generateRandom() * (high - low);
}

//--------------------------------------------------------------------------------

Pos positionUpdateCheck(Pos actualPos)
{

    Pos Pnew = actualPos;

    if (Pnew.x < xmin)
    {
        Pnew.x = xmin;
    }

    if (Pnew.x > xmax)
    {
        Pnew.x = xmax;
    }

    if (Pnew.y < ymin)
    {
        Pnew.y = ymin;
    }

    if (Pnew.y > ymax)
    {
        Pnew.y = ymax;
    }

    if (Pnew.z < zmin)
    {
        Pnew.z = zmin;
    }

    if (Pnew.z > zmax)
    {
        Pnew.z = zmax;
    }

    return Pnew;
}
//--------------------------------------------------------------------------------

std::vector<std::vector<Pos>> initPosXY()
{

    std::vector<std::vector<Pos>> pos;

    for (int ii = 0; ii < BIRDS; ii++)
    {
        std::vector<Pos> posi;
        for (int jj = 0; jj < BIRDS; jj++)
        {

            if (ii == jj)
            {

                posi.push_back({-INF, -INF, -INF});
            }

            posi.push_back({valueGenerator(xmin, xmax), valueGenerator(ymin, ymax), valueGenerator(zmin, zmax)});
        }
        pos.push_back(posi);
    }

    return pos;
}

//--------------------------------------------------------------------------------

std::vector<float> functionInit(std::vector<std::vector<Pos>> pos)
{

    std::vector<float> funcValue;

    for (int jj = 0; jj < BIRDS; jj++)
    {
        auto funcValuei = 0;

        for (int ii = 0; ii < BIRDS; ii++)
        {

            if (jj != ii)
            {
                float p1{0}, p2{0}, p3{0}, p4{0};
                auto f1 = 1 - (float)(std::pow(pos[jj][ii].y, 2) * pos[jj][ii].z) / (float)(71785.0 * std::pow(pos[jj][ii].x, 4));

                auto f2 = (float)(4.0 * std::pow(pos[jj][ii].y, 2) - pos[jj][ii].x * pos[jj][ii].y) / (float)(12556.0 * (pos[jj][ii].y * std::pow(pos[jj][ii].x, 3) - std::pow(pos[jj][ii].x, 4))) + 1.0 / (float)(5108.0 * std::pow(pos[jj][ii].x, 2));

                auto f3 = 1.0 - (float)(140.45 * pos[jj][ii].x) / (float)(std::pow(pos[jj][ii].y, 2) * pos[jj][ii].z);

                auto f4 = ((float)(pos[jj][ii].x + pos[jj][ii].y) / (float)1.5) - 1.0;

                if (f1 > 0)
                {
                    p1 = std::pow(f1, 2);
                }
                if (f2 > 0)
                {
                    p2 = std::pow(f2, 2);
                }
                if (f3 > 0)
                {
                    p3 = std::pow(f3, 2);
                }

                if (f4 > 0)
                {
                    p4 = std::pow(f4, 2);
                }

                funcValuei += (pos[jj][ii].z + 2) * pos[jj][ii].y * std::pow(pos[jj][ii].x, 2) + R * p1 + R * p2 + R * p3 + R * p4;
            }
        }

        funcValue.push_back(funcValuei);
    }

    return funcValue;
}

//--------------------------------------------------------------------------------

float functionUpdate(std::vector<Pos> pos)
{

    auto fitValue = 0.0f;

    for (auto &ii : pos)
    {
        if (ii.x != -INF && ii.y != -INF)
        {
            float p1{0}, p2{0}, p3{0}, p4{0};
            auto f1 = 1 - (float)(std::pow(ii.y, 2) * ii.z) / (float)(71785.0 * std::pow(ii.x, 4));

            auto f2 = (float)(4.0 * std::pow(ii.y, 2) - ii.x * ii.y) / (float)(12556.0 * (ii.y * std::pow(ii.x, 3) - std::pow(ii.x, 4))) + 1.0 / (float)(5108.0 * std::pow(ii.x, 2));

            auto f3 = 1.0 - (float)(140.45 * ii.x) / (float)(std::pow(ii.y, 2) * ii.z);

            auto f4 = ((float)(ii.x + ii.y) / (float)1.5) - 1.0;

            if (f1 > 0)
            {
                p1 = std::pow(f1, 2);
            }
            if (f2 > 0)
            {
                p2 = std::pow(f2, 2);
            }
            if (f3 > 0)
            {
                p3 = std::pow(f3, 2);
            }

            if (f4 > 0)
            {
                p4 = std::pow(f4, 2);
            }

            fitValue += (ii.z + 2) * ii.y * std::pow(ii.x, 2) + R * p1 + R * p2 + R * p3 + R * p4;
        }
    }

    return fitValue;
}

//-------------------------------------------------------------------------------

int findBestValueMigration(std::vector<float> func)
{

    auto max = 0.0;
    auto index = 0;
    int ix = 0;

    for (auto &ii : func)
    {
        if (ii > max)
        {
            max = ii;
            index = ix;
        }
        ix++;
    }

    return index;
}
//--------------------------------------------------------------------------------
std::vector<Pos> positionUpdateGF(std::vector<Pos> actual, Pos best, Pos direction)
{

    std::vector<Pos> pos;

    for (int ii = 0; ii < actual.size(); ii++)
    {
        Pos posi;
        posi.x = best.x + generateRandom() * direction.x * (best.x - actual[ii].x);
        posi.y = best.y + generateRandom() * direction.y * (best.y - actual[ii].y);
        posi.z = best.z + generateRandom() * direction.z * (best.z - actual[ii].z);
        posi = positionUpdateCheck(posi);
        pos.push_back(posi);
    }

    return pos;
}
//--------------------------------------------------------------------------------
std::vector<Pos> positionUpdateTF(std::vector<Pos> actual, Pos direction)
{

    std::vector<Pos> pos;

    for (int ii = 0; ii < actual.size(); ii++)
    {
        Pos posi;
        posi.x = actual[ii].x + generateRandom() * direction.x * actual[ii].x;
        posi.y = actual[ii].y + generateRandom() * direction.y * actual[ii].y;
        posi.z = actual[ii].z + generateRandom() * direction.z * actual[ii].z;
        posi = positionUpdateCheck(posi);
        pos.push_back(posi);
    }

    return pos;
}
//--------------------------------------------------------------------------------

std::tuple<int, int> bestFoodSource(std::vector<int> vec)
{

    int index = 1;
    int bestValue = 0;

    for (int ii = 0; ii < vec.size(); ii++)
    {

        if (vec[ii] > bestValue)
        {
            bestValue = vec[ii];
            index = ii;
        }
    }

    return std::make_tuple(index, bestValue);
}
//--------------------------------------------------------------------------------

int maxVecValue(std::vector<int> vec)
{

    int max = 0;
    for (auto &ii : vec)
    {
        if (ii > max)
        {
            max = ii;
        }
    }
    return max;
}

//--------------------------------------------------------------------------------

std::vector<std::vector<int>> updateVisitTable(std::vector<std::vector<int>> vt, int bird, int index, int loop)
{

    std::vector<int> temp;

    std::for_each(vt[bird].begin(), vt[bird].end(), [&](int i)
                  { temp.push_back(i + 1); });

    vt[bird] = temp;

    if (loop != 2 && loop != 3 && loop != 4)
    {
        vt[bird][index] = 0;
    }

    if (loop != 0 && loop != 3)
    {
        std::vector<int> maxVec;
        for (auto &ii : vt)
        {
            maxVec.push_back(maxVecValue(ii) + 1);
        }

        int ix = 0;
        for (auto &ii : vt)
        {

            ii[bird] = maxVec[ix];
            ix++;
        }

        vt[bird][bird] = -INF;
    }
    return vt;
}

//--------------------------------------------------------------------------------

float computeBestPos(Pos ii)
{

    auto fitValue = 10000000.0f;

    if (ii.x != -INF || ii.y != -INF || ii.z != -INF)
    {
        float p1{0}, p2{0}, p3{0}, p4{0};
        auto f1 = 1 - (float)(std::pow(ii.y, 2) * ii.z) / (float)(71785.0 * std::pow(ii.x, 4));

        auto f2 = (float)(4.0 * std::pow(ii.y, 2) - ii.x * ii.y) / (float)(12556.0 * (ii.y * std::pow(ii.x, 3) - std::pow(ii.x, 4))) + 1.0 / (float)(5108.0 * std::pow(ii.x, 2));

        auto f3 = 1.0 - (float)(140.45 * ii.x) / (float)(std::pow(ii.y, 2) * ii.z);

        auto f4 = ((float)(ii.x + ii.y) / (float)1.5) - 1.0;

        if (f1 > 0)
        {
            p1 = std::pow(f1, 2);
        }
        if (f2 > 0)
        {
            p2 = std::pow(f2, 2);
        }
        if (f3 > 0)
        {
            p3 = std::pow(f3, 2);
        }

        if (f4 > 0)
        {
            p4 = std::pow(f4, 2);
        }

        fitValue = (ii.z + 2) * ii.y * std::pow(ii.x, 2) + R * p1 + R * p2 + R * p3 + R * p4;
    }
    return fitValue;
}

//--------------------------------------------------------------------------------

std::pair<Pos, float> chooseBestFit(std::vector<std::vector<Pos>> pos, std::vector<float> func)
{

    float min = INF;
    int index = 0;
    int ix = 0;

    for (auto &ii : func)
    {

        if (ii < min)
        {

            min = ii;
            index = ix;
        }
        ix++;
    }

    auto bestVec = pos[index];

    Pos bestPos;
    float minFun = 100000000000.0f;

    for (int ii = 0; ii < bestVec.size(); ii++)
    {

        if (computeBestPos(bestVec[ii]) < minFun)
        {

            minFun = computeBestPos(bestVec[ii]);
            bestPos = bestVec[ii];
        }
    }
    return std::make_pair(bestPos, minFun);
}

//--------------------------------------------------------------------------------

void runAHA()
{

    std::vector<std::vector<Pos>> currentPos = initPosXY();
    std::vector<float> currentfuncValue = functionInit(currentPos);
    std::vector<int> vti(BIRDS, 0);
    std::vector<std::vector<int>> visitTable(BIRDS, vti);

    for (int jj = 0; jj < EVOLUTIONS; jj++)
    {
        for (int ii = 0; ii < BIRDS; ii++)
        {
            Pos direction_vector{0.0f, 0.0f, 0.0f};
            auto r = generateRandom();

            // Diagonal flight

            if (r < 1.0 / 3.0)
            {

                auto rand_num = generateRandomPerm();

                if (rand_num == 0)
                {
                    direction_vector = {1.0, 0.0, 1.0};
                }
                if (rand_num == 1)
                {
                    direction_vector = {0.0, 1.0, 1.0};
                }
                if (rand_num == 2)
                {
                    direction_vector = {1.0, 1.0, 1.0};
                }
            }

            // Omdirectional flight
            if (r > 2.0 / 3.0)
            {
                direction_vector = {1.0, 1.0, 1.0};
            }

            // Axial flight

            else
            {
                auto rand_num = generateRandomPerm();

                if (rand_num == 0)
                {
                    direction_vector = {1.0, 0.0, 1.0};
                }
                
                if (rand_num == 1)
                {
                    direction_vector = {0.0, 1.0, 1.0};
                }
                
                if (rand_num == 2)
                {
                    direction_vector = {1.0, 1.0, 1.0};
                }
            }

            // Guided foraging
            if (generateRandom() < 0.5)
            {

                auto bestIndexValue = bestFoodSource(visitTable[ii]);
                // std::cout << "check " << "\n";
                auto newPositions = positionUpdateGF(currentPos[ii], currentPos[ii][std::get<0>(bestIndexValue)], direction_vector);

                auto newValueFunction = functionUpdate(newPositions);

                if (newValueFunction < currentfuncValue[ii])
                {

                    currentfuncValue[ii] = newValueFunction;
                    currentPos[ii] = newPositions;
                    visitTable = updateVisitTable(visitTable, ii, std::get<0>(bestIndexValue), 1);
                }

                else
                {
                    visitTable = updateVisitTable(visitTable, ii, std::get<0>(bestIndexValue), 0);
                }
            }

            // Territorial foraging
            else
            {

                auto newPositions = positionUpdateTF(currentPos[ii], direction_vector);
                auto newValueFunction = functionUpdate(newPositions);

                if (newValueFunction < currentfuncValue[ii])
                {

                    currentfuncValue[ii] = newValueFunction;
                    currentPos[ii] = newPositions;
                    visitTable = updateVisitTable(visitTable, ii, 0, 2);
                }
                else
                {

                    visitTable = updateVisitTable(visitTable, ii, 0, 3);
                }
            }
        }
        // Migration foraging
        if (jj % MOD == 0)
        {
            auto indexMigration = findBestValueMigration(currentfuncValue);
            // std::cout << "index = " << indexMigration << "\n";
            currentPos[indexMigration] = initPosXY()[indexMigration];

            visitTable = updateVisitTable(visitTable, indexMigration, 0, 4);
            currentfuncValue[indexMigration] = functionUpdate(currentPos[indexMigration]);
        }
    }

    auto bestPosValueSCA = chooseBestFit(currentPos, currentfuncValue);

    std::cout << "-------Optimization problem: tension/compression spring ----------\n";
    std::cout << "min weight= " << std::get<1>(bestPosValueSCA) << "\n";
    std::cout << "values   d= " << std::get<0>(bestPosValueSCA).x << " D= " << std::get<0>(bestPosValueSCA).y << " N= " << std::get<0>(bestPosValueSCA).z << "\n";
}

int main()
{

    runAHA();
}
