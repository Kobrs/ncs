#pragma once

#ifndef SWIG
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#endif // SWIG

#include <ncs/spec/ModelParameters.h>

namespace ncs {

namespace com {
struct Generator;
}

namespace spec {

typedef std::ranlux48_base RNG;

class ModelParameters;

/**
  Generates output values given a random number generator. Its exact behavior
  is governed by the implementation.

  Note that some output types cannot be generated with particular types of
  Generators. For example, a generator that outputs numbers from a normal
  distribution should not be expected to generate strings. In these cases, any
  attempt to generate invalid output will result in an error.
*/
class Generator {
public:
  /**
    Generates a string.

    @param rng A random number generator.
    @return A string.
  */
  virtual std::string generateString(RNG* rng);

  /**
    Generates an integer.

    @param rng A random number generator.
    @return A 64-bit integer.
  */
  virtual long generateInt(RNG* rng);

  /**
    Generates a floating point value.

    @param rng A random number generator.
    @return A 64-bit floating point value.
  */
  virtual double generateDouble(RNG* rng);

  /**
    Generates a list of generators.
    @param rng A random number generator.
    @return A vector of generators.
  */
  virtual std::vector<Generator*> generateList(RNG* rng);

  virtual ModelParameters* generateParameters(RNG* rng);

  /**
    Returns the name of the generator. Useful for printing error messages.

    @return The name of this specific generator type.
  */
  virtual const std::string& name() const;

  inline Generator* base() {
    return this;
  }

  virtual bool makeProtobuf(com::Generator* gen) const = 0;

  static Generator* fromProtobuf(com::Generator* gen);
};

} // namespace spec

} // namespace ncs
