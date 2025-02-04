// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OPENSSL_OBJECT_WRAPPER_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OPENSSL_OBJECT_WRAPPER_H_

#include <functional>
#include <memory>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Templated base wrapper class to be used for OpenSSL objects that require custom deleter and copier functions.
 *
 * @tparam ObjectType The type of OpenSSL object this class should wrap.
 * @tparam CopierFunc The copier type (function signature) for the OpenSSL object.
 */
template<typename ObjectType, typename CopierFunc = std::function<ObjectType*(const ObjectType*)>>
class OpenSSLObjectWrapper
{
public:
  virtual ~OpenSSLObjectWrapper() = default;

  /**
   * Copy constructor.
   *
   * @param other The OpenSSLObjectWrapper object to copy.
   */
  OpenSSLObjectWrapper(const OpenSSLObjectWrapper& other)
    : mObject({ other.mCopier(other.mObject.get()), other.mObject.get_deleter() })
    , mCopier(other.mCopier)
  {
  }

  /**
   * Copy assignment operator.
   *
   * @param other The OpenSSLObjectWrapper object to copy.
   */
  OpenSSLObjectWrapper& operator=(const OpenSSLObjectWrapper& other)
  {
    if (this != &other)
    {
      mObject = { other.mCopier(other.mObject.get()), other.mObject.get_deleter() };
      mCopier = other.mCopier;
    }

    return *this;
  }

  OpenSSLObjectWrapper(OpenSSLObjectWrapper&&) noexcept = default;
  OpenSSLObjectWrapper& operator=(OpenSSLObjectWrapper&&) noexcept = default;

  /**
   * Get the wrapped OpenSSL object.
   *
   * @return A pointer to the wrapped OpenSSL object. nullptr if no object exists.
   */
  [[nodiscard]] ObjectType* get() { return mObject.get(); }
  [[nodiscard]] const ObjectType* get() const { return mObject.get(); }

  /**
   * Release ownership of the wrapped OpenSSL object. This will cause memory leaks if ownership has not already been
   * taken by another object.
   */
  void release() { mObject.release(); }

  /**
   * Determine if this OpenSSLObjectWrapper has a valid OpenSSL object.
   *
   * @return \c TRUE if there exists an OpenSSL object, otherwise \c FALSE.
   */
  [[nodiscard]] explicit operator bool() const { return mObject != nullptr; }

protected:
  OpenSSLObjectWrapper() = default;

  /**
   * Construct with values for the object, its custom deleter, and optionally a custom copier.
   *
   * @param object  The OpenSSL object to wrap.
   * @param deleter The deleter function for the OpenSSL object.
   * @param copier  The copier function for the OpenSSL object.
   */
  OpenSSLObjectWrapper(ObjectType* object,
                       const std::function<void(ObjectType*)>& deleter,
                       const CopierFunc& copier = CopierFunc())
    : mObject({ object, deleter })
    , mCopier(copier)
  {
  }

private:
  /**
   * Pointer to the OpenSSL object with its associated deleter.
   */
  std::unique_ptr<ObjectType, std::function<void(ObjectType*)>> mObject = { nullptr,
                                                                            std::function<void(ObjectType*)>() };

  /**
   * The copier function to use to copy the wrapped OpenSSL object.
   */
  CopierFunc mCopier;
};

} // namespace Hiero::internal::Utils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OPENSSL_OBJECT_WRAPPER_H_
