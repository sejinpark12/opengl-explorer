#include "Base/maths_utils.h"

const float MathsUtils::x(const vertex *v)
{
    return (*v)[0];
}

const float MathsUtils::y(const vertex *v)
{
    return (*v)[1];
}

const float MathsUtils::z(const vertex *v)
{
    return (*v)[2];
}

const float MathsUtils::r(const vertex *v)
{
    return (*v)[3];
}

const float MathsUtils::g(const vertex *v)
{
    return (*v)[4];
}

const float MathsUtils::b(const vertex *v)
{
    return (*v)[5];
}

const unsigned int MathsUtils::getNbVertex(const MathsUtils::vertex vertices[])
{
    return (unsigned int)(sizeof(*vertices) / MathsUtils::VERTEX_ELEMENTS_NB);
}

const unsigned int MathsUtils::getNbElements(const MathsUtils::vertex vertices[])
{
    return MathsUtils::getNbVertex(vertices) * VERTEX_ELEMENTS_NB;
}

std::vector<float> MathsUtils::duplicate(std::vector<std::vector<float>> array, bool mirror)
{
    std::vector<float> result;
    if (mirror) {
        for (int i = 0; i < array.size(); i++) {
            result.push_back(-1);
            result.push_back(1);     
        }        
    }
    else {
        for (int i = 0; i < array.size(); i++) {
            result.push_back(array[i][0]);
            result.push_back(array[i][1]);
            result.push_back(array[i][2]);
            result.push_back(array[i][0]);
            result.push_back(array[i][1]);
            result.push_back(array[i][2]);        
        }
    }
    return result;
}

std::vector<uint16_t> MathsUtils::createIndices(int length)
{
    std::vector<uint16_t> indices(length * 6);
    int c = 0, index = 0;
    for (int j = 0; j < length; j++) {
      int i = index;
      indices[c++] = i + 0; 
      indices[c++] = i + 1;
      indices[c++] = i + 2;
      indices[c++] = i + 2;
      indices[c++] = i + 1;
      indices[c++] = i + 3;
      index += 2;
    }
    return indices;
}

std::vector<std::vector<float>> MathsUtils::relative(std::vector<std::vector<float>> array, int offset)
{
    std::vector<std::vector<float>> result(array.size(), std::vector<float>(3, 0));

    for (int i = 0; i < array.size(); i++) {
        int index = MathsUtils::clamp(i + offset, 0, array.size() - 1);
        result[i][0] = array[index][0];
        result[i][1] = array[index][1];
        result[i][2] = array[index][2];
    }
    return result;
}

int MathsUtils::clamp(int value, int begin, int end)
{
    if (value > end)
        return end;
    else if (value < begin)
        return begin;
    return value;
}