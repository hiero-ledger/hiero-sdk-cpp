// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_CRYPTO_OBJECT_WRAPPER_H_
#define HIERO_SDK_CPP_IMPL_CRYPTO_OBJECT_WRAPPER_H_

#include <functional>
#include <memory>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Templated base wrapper class to be used for crypto library objects that require custom deleter and copier functions.
 *
 * @tparam ObjectType The type of crypto object this class should wrap.
 * @tparam CopierFunc The copier type (function signature) for the crypto object.
 */
template<typename ObjectType, typename CopierFunc = std::function<ObjectType*(const ObjectType*)>>
class CryptoObjectWrapper
{
public:
  virtual ~CryptoObjectWrapper() = default;

  /**
   * Copy constructor.
   *
   * @param other The CryptoObjectWrapper object to copy.
   */
  CryptoObjectWrapper(const CryptoObjectWrapper& other)
    : mObject({ other.mCopier(other.mObject.get()), other.mObject.get_deleter() })
    , mCopier(other.mCopier)
  {
  }

  /**
   * Copy assignment operator.
   *
   * @param other The CryptoObjectWrapper object to copy.
   */
  CryptoObjectWrapper& operator=(const CryptoObjectWrapper& other)
  {
    if (this != &other)
    {
      mObject = { other.mCopier(other.mObject.get()), other.mObject.get_deleter() };
      mCopier = other.mCopier;
    }

    return *this;
  }

  CryptoObjectWrapper(CryptoObjectWrapper&&) noexcept = default;
  CryptoObjectWrapper& operator=(CryptoObjectWrapper&&) noexcept = default;

  /**
   * Get the wrapped crypto object.
   *
   * @return A pointer to the wrapped crypto object. nullptr if no object exists.
   */
  [[nodiscard]] ObjectType* get() { return mObject.get(); }
  [[nodiscard]] const ObjectType* get() const { return mObject.get(); }

  /**
   * Release ownership of the wrapped crypto object. This will cause memory leaks if ownership has not already been
   * taken by another object.
   */
  void release() { mObject.release(); }

  /**
   * Determine if this CryptoObjectWrapper has a valid crypto object.
   *
   * @return \c TRUE if there exists a crypto object, otherwise \c FALSE.
   */
  [[nodiscard]] explicit operator bool() const { return mObject != nullptr; }

protected:
  CryptoObjectWrapper() = default;

  /**
   * Construct with values for the object, its custom deleter, and optionally a custom copier.
   *
   * @param object  The crypto object to wrap.
   * @param deleter The deleter function for the crypto object.
   * @param copier  The copier function for the crypto object.
   */
  CryptoObjectWrapper(ObjectType* object,
                      const std::function<void(ObjectType*)>& deleter,
                      const CopierFunc& copier = CopierFunc())
    : mObject({ object, deleter })
    , mCopier(copier)
  {
  }

private:
  /**
   * Pointer to the crypto object with its associated deleter.
   */
  std::unique_ptr<ObjectType, std::function<void(ObjectType*)>> mObject = { nullptr,
                                                                            std::function<void(ObjectType*)>() };

  /**
   * The copier function to use to copy the wrapped crypto object.
   */
  CopierFunc mCopier;
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_CRYPTO_OBJECT_WRAPPER_H_
