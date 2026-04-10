#include "BmnEventCentrality.h"

BmnEventCentrality::BmnEventCentrality() : TNamed(), fClassProbability() {}

BmnEventCentrality::BmnEventCentrality(const BmnEventCentrality& other) : TNamed(other), fClassProbability(other.fClassProbability) {
  // Copy constructor: copies the class probability set
}

BmnEventCentrality& BmnEventCentrality::operator=(const BmnEventCentrality& other) {
  if (this != &other) {
    TNamed::operator=(other);
    fClassProbability = other.fClassProbability; // Copy the class probability set
  }
  return *this;
}

BmnEventCentrality::~BmnEventCentrality() {
  // Destructor: nothing to do as the set is cleaned up automatically
}

int BmnEventCentrality::GetClass() const {
  return GetClass(BmnCentralityClass::Method::FHCalHodo); // Delegate to the method-specific function
}

float BmnEventCentrality::GetProbability() const {
  return GetProbability(BmnCentralityClass::Method::FHCalHodo); // Delegate to the method-specific function
}

const std::vector<float>& BmnEventCentrality::GetSoftCentrality() const {
  return GetSoftCentrality(BmnCentralityClass::Method::FHCalHodo); // Delegate to the method-specific function
}

std::pair<int, float> BmnEventCentrality::GetClassProbabilityPair(BmnCentralityClass::Method method) const {
  int candida = -1;
  float proba = -1.0;
  for (const auto& cls : fClassProbability) {
    if (cls.GetMethod() == method) {
      if (cls.GetProbability() > proba) {
        proba = cls.GetProbability();
        candida = cls.GetIndex();
      }
    }
  }
  return std::make_pair(candida, proba);
}

int BmnEventCentrality::GetClass(BmnCentralityClass::Method method) const {
  return GetClassProbabilityPair(method).first;
}

float BmnEventCentrality::GetProbability(BmnCentralityClass::Method method) const {
  return GetClassProbabilityPair(method).second;
}

const std::vector<float>& BmnEventCentrality::GetSoftCentrality(BmnCentralityClass::Method method) const {
  // Convert fClassProbability to a std::vector<float>
  static std::vector<float> probabilities;
  probabilities.clear();
  for (const auto& cls : fClassProbability) {
    probabilities.push_back(cls.GetProbability());
  }
  return probabilities;
}

void BmnEventCentrality::SetCentrality(int cluster_id, float probability, BmnCentralityClass::Method method) {
  // Add or update the class probability for the specified method
  fClassProbability.insert({ cluster_id, probability, method });
}

void BmnEventCentrality::Reset() {
  fClassProbability.clear(); // Clear the class probability set
}
