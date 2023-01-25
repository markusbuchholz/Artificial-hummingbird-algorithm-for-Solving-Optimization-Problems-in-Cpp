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

float xmin = -5.0;
float xmax = 5.0;
float ymin = -5.0;
float ymax = 5.0;
int BIRDS = 50;
int EVOLUTIONS = 200;
int MOD = 2 * BIRDS;
//--------------------------------------------------------------------------------

struct Pos
{

    float x;
    float y;
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
    std::uniform_int_distribution<int> distribution(0, 1);
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

    return Pnew;
}
//--------------------------------------------------------------------------------

std::vector<std::vector<Pos>> initPosXY()
{
    // std::cout << __FUNCTION__ << "\n";

    std::vector<std::vector<Pos>> pos;

    for (int ii = 0; ii < BIRDS; ii++)
    {
        std::vector<Pos> posi;
        for (int jj = 0; jj < BIRDS; jj++)
        {

            if (ii == jj)
            {

                posi.push_back({-INF, -INF});
            }

            posi.push_back({valueGenerator(xmin, xmax), valueGenerator(ymin, ymax)});
        }
        pos.push_back(posi);
    }

    return pos;
}

//--------------------------------------------------------------------------------

std::vector<float> functionInit(std::vector<std::vector<Pos>> pos)
{

    std::vector<float> funcValue;

    for (int ii = 0; ii < BIRDS; ii++)
    {
        auto funcValuei = 0;

        for (int jj = 0; jj < BIRDS; jj++)
        {

            if (ii != jj)
            {

                funcValuei += (pos[ii][jj].x * pos[ii][jj].y);
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
        if (ii.x != -INF || ii.y != -INF)
        {

            fitValue += (ii.x * ii.y);
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

std::vector<Pos> chooseBestFit(std::vector<std::vector<Pos>> pos, std::vector<float> func)
{

    auto minf = INF;
    auto index = 0;
    int ix = 0;

    for (auto &ii : func)
    {

        if (ii < minf)
        {
            minf = ii;
            index = ix;
        }
        ix++;
    }

    return pos[index];
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
            Pos direction_vector{0.0f, 0.0f};
            auto r = generateRandom();

            // Diagonal flight

            if (r < 1.0 / 3.0)
            {

                auto rand_num = generateRandomPerm();

                if (rand_num == 0)
                {
                    direction_vector = {1.0, 0.0};
                }
                if (rand_num == 1)
                {
                    direction_vector = {0.0, 1.0};
                }
            }

            // Omdirectional flight
            if (r > 2.0 / 3.0)
            {
                direction_vector = {1.0, 1.0};
            }

            // Axial flight

            else
            {
                auto rand_num = generateRandomPerm();

                if (rand_num == 0)
                {
                    direction_vector = {1.0, 0.0};
                }
                if (rand_num == 1)
                {
                    direction_vector = {0.0, 1.0};
                }
            }

            // Guided foraging
            if (generateRandom() < 0.5)
            {

                auto bestIndexValue = bestFoodSource(visitTable[ii]);
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
            currentPos[indexMigration] = initPosXY()[indexMigration];

            visitTable = updateVisitTable(visitTable, indexMigration, 0, 4);
            currentfuncValue[indexMigration] = functionUpdate(currentPos[indexMigration]);
        }
    }

    auto bestPos = chooseBestFit(currentPos, currentfuncValue);

    for (auto &ii : bestPos)
    {

        std::cout << ii.x << ", " << ii.y << "\n";
    }
}

int main()
{

    runAHA();
}
