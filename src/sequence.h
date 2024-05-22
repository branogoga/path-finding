#pragma once

template <typename ValueType>
class LinearSequence
{
 public:
  LinearSequence(ValueType first = static_cast<ValueType>(0), ValueType step = static_cast<ValueType>(1))
      : start(first), step(step), next(first), index(0)
  {
  }

  ValueType operator()()
  {
    ++index;
    ValueType result = next;
    next += step;
    return result;
  }

 private:
  ValueType start;
  ValueType step;
  ValueType next;
  unsigned index;
};

template <typename ValueType>
class AlternateSequence
{
 public:
  AlternateSequence(ValueType value = static_cast<ValueType>(1)) : value(value), next(value), index(0)
  {
  }

  ValueType operator()()
  {
    ++index;
    ValueType result = next;
    next *= static_cast<ValueType>(-1);
    return result;
  }

 private:
  ValueType value;
  ValueType next;
  unsigned index;
};