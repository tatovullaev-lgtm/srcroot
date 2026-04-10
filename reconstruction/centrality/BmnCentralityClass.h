/** \file BmnCentralityClass.h
 ** \author Nikolay Karpushkin
 ** \date 07.03.2024
 **/

#ifndef BMNCENTRALITYCLASS_H
#define BMNCENTRALITYCLASS_H

#include "Rtypes.h"   // Include necessary ROOT header

#include <cstdint>   // Include necessary headers

class BmnCentralityClass
{
  public:
    enum class Method
    {
        FHCalHodo,
    };

    BmnCentralityClass();                                              // Constructor
    BmnCentralityClass(int index, float probability, Method method);   // Constructor with parameters
    BmnCentralityClass(const BmnCentralityClass& other);               // Copy constructor
    BmnCentralityClass(BmnCentralityClass&& other);                    // Move constructor
    BmnCentralityClass& operator=(const BmnCentralityClass& other);    // Copy assignment operator
    BmnCentralityClass& operator=(BmnCentralityClass&& other);         // Move assignment operator
    bool operator<(const BmnCentralityClass& other) const;

    virtual ~BmnCentralityClass();   // Destructor

    // Getters and Setters for private members
    int GetIndex() const;
    void SetIndex(int index);
    float GetProbability() const;
    void SetProbability(float probability);
    Method GetMethod() const;
    void SetMethod(Method method);

    virtual const char* GetClassName() { return "BmnCentralityClass"; }   // Virtual method
    void reset();                                                         // Reset method

  private:
    int index;
    float probability;
    Method method;

    ClassDef(BmnCentralityClass, 1);
};

#endif /* BMNCENTRALITYCLASS_H */
