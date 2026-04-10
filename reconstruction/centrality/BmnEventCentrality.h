#ifndef BMNEVENTCENRALITY_H
#define BMNEVENTCENRALITY_H

#include <set>
#include <vector>
#include <TNamed.h>
#include <BmnCentralityClass.h>

class BmnEventCentrality : public TNamed {
public:
    // Comparison operator for Class
    struct Comparator {
        bool operator()(const BmnCentralityClass& lhs, const BmnCentralityClass& rhs) const {
            if (lhs.GetIndex() != rhs.GetIndex()) {
                return lhs.GetIndex() < rhs.GetIndex();
            }
            return static_cast<int>(lhs.GetMethod()) < static_cast<int>(rhs.GetMethod());
        }
    };

    BmnEventCentrality();
    BmnEventCentrality(const BmnEventCentrality& other);
    BmnEventCentrality& operator=(const BmnEventCentrality& other);
    virtual ~BmnEventCentrality();

    int GetClass() const; // Return index of most probable class with default method
    float GetProbability() const; // Return index of most probable class with default method
    const std::vector<float>& GetSoftCentrality() const; // Return vector of probabilities of all classes with default method

    int GetClass(BmnCentralityClass::Method method) const; // Return index of most probable class with selected method
    float GetProbability(BmnCentralityClass::Method method) const; // Return index of most probable class with selected method
    const std::vector<float>& GetSoftCentrality(BmnCentralityClass::Method method) const; // Return vector of probabilities of all classes with selected method

    void SetCentrality(int cluster_id, float probability, BmnCentralityClass::Method method);

    void Reset();

private:
    std::set<BmnCentralityClass, Comparator> fClassProbability;
    std::pair<int, float> GetClassProbabilityPair(BmnCentralityClass::Method method) const; // Return pair of most probable class with selected method

    ClassDef(BmnEventCentrality, 1);
};

#endif // BMNEVENTCENRALITY_H
