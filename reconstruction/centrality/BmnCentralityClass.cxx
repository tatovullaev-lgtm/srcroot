/** \file BmnCentralityClass.cpp
 ** \author Nikolay Karpushkin
 ** \date 07.03.2024
 **/

#include "BmnCentralityClass.h"

#include <utility>

// Constructor
BmnCentralityClass::BmnCentralityClass()
    : index(0)
    , probability(0.0)
    , method(Method::FHCalHodo)
{}

// Constructor with parameters
BmnCentralityClass::BmnCentralityClass(int index_, float probability_, Method method_)
    : index(index_)
    , probability(probability_)
    , method(method_)
{}

// Copy constructor
BmnCentralityClass::BmnCentralityClass(const BmnCentralityClass& other)
    : index(other.index)
    , probability(other.probability)
    , method(other.method)
{}

// Move constructor
BmnCentralityClass::BmnCentralityClass(BmnCentralityClass&& other)
    : index(std::move(other.index))
    , probability(std::move(other.probability))
    , method(std::move(other.method))
{}

// Copy assignment operator
BmnCentralityClass& BmnCentralityClass::operator=(const BmnCentralityClass& other)
{
    if (this != &other) {
        index = other.index;
        probability = other.probability;
        method = other.method;
    }
    return *this;
}

// Move assignment operator
BmnCentralityClass& BmnCentralityClass::operator=(BmnCentralityClass&& other)
{
    if (this != &other) {
        index = std::move(other.index);
        probability = std::move(other.probability);
        method = std::move(other.method);
    }
    return *this;
}

bool BmnCentralityClass::operator<(const BmnCentralityClass& other) const
{
    if (this->GetIndex() != other.GetIndex()) {
        return this->GetIndex() < other.GetIndex();
    }
    return static_cast<int>(this->GetMethod()) < static_cast<int>(other.GetMethod());
}

// Destructor
BmnCentralityClass::~BmnCentralityClass() {}

// Getter for index
int BmnCentralityClass::GetIndex() const
{
    return index;
}

// Setter for index
void BmnCentralityClass::SetIndex(int index_)
{
    this->index = index_;
}

// Getter for probability
float BmnCentralityClass::GetProbability() const
{
    return probability;
}

// Setter for probability
void BmnCentralityClass::SetProbability(float probability_)
{
    this->probability = probability_;
}

// Getter for method
BmnCentralityClass::Method BmnCentralityClass::GetMethod() const
{
    return method;
}

// Setter for method
void BmnCentralityClass::SetMethod(Method method_)
{
    this->method = method_;
}

// Reset method
void BmnCentralityClass::reset()
{
    index = 0;
    probability = 0.0;
    method = Method::FHCalHodo;
}
