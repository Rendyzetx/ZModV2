#ifndef GIOVANNI_DICANIO_WINREG_HPP_INCLUDED
#define GIOVANNI_DICANIO_WINREG_HPP_INCLUDED

#include <Windows.h>
#include <crtdbg.h>

#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <variant>
#include <vector>

namespace winreg
{

class RegException;
class RegResult;

template <typename T>
class RegExpected;

class RegKey
{
public:

    RegKey() noexcept = default;

    explicit RegKey(HKEY hKey) noexcept;

    RegKey(HKEY hKeyParent, const std::wstring& subKey);

    RegKey(HKEY hKeyParent, const std::wstring& subKey, REGSAM desiredAccess);

    RegKey(RegKey&& other) noexcept;

    RegKey& operator=(RegKey&& other) noexcept;

    RegKey(const RegKey&) = delete;
    RegKey& operator=(const RegKey&) = delete;

    ~RegKey() noexcept;

    [[nodiscard]] HKEY Get() const noexcept;

    [[nodiscard]] bool IsValid() const noexcept;

    [[nodiscard]] explicit operator bool() const noexcept;

    [[nodiscard]] bool IsPredefined() const noexcept;

    void Close() noexcept;

    [[nodiscard]] HKEY Detach() noexcept;

    void Attach(HKEY hKey) noexcept;

    void SwapWith(RegKey& other) noexcept;

    void Create(
        HKEY hKeyParent,
        const std::wstring& subKey,
        REGSAM desiredAccess = KEY_READ | KEY_WRITE | KEY_WOW64_64KEY
    );

    void Create(
        HKEY hKeyParent,
        const std::wstring& subKey,
        REGSAM desiredAccess,
        DWORD options,
        SECURITY_ATTRIBUTES* securityAttributes,
        DWORD* disposition
    );

    void Open(
        HKEY hKeyParent,
        const std::wstring& subKey,
        REGSAM desiredAccess = KEY_READ | KEY_WRITE | KEY_WOW64_64KEY
    );

    [[nodiscard]] RegResult TryCreate(
        HKEY hKeyParent,
        const std::wstring& subKey,
        REGSAM desiredAccess = KEY_READ | KEY_WRITE | KEY_WOW64_64KEY
    ) noexcept;

    [[nodiscard]] RegResult TryCreate(
        HKEY hKeyParent,
        const std::wstring& subKey,
        REGSAM desiredAccess,
        DWORD options,
        SECURITY_ATTRIBUTES* securityAttributes,
        DWORD* disposition
    ) noexcept;

    [[nodiscard]] RegResult TryOpen(
        HKEY hKeyParent,
        const std::wstring& subKey,
        REGSAM desiredAccess = KEY_READ | KEY_WRITE | KEY_WOW64_64KEY
    ) noexcept;

    void SetDwordValue(const std::wstring& valueName, DWORD data);
    void SetQwordValue(const std::wstring& valueName, const ULONGLONG& data);
    void SetStringValue(const std::wstring& valueName, const std::wstring& data);
    void SetExpandStringValue(const std::wstring& valueName, const std::wstring& data);
    void SetMultiStringValue(const std::wstring& valueName, const std::vector<std::wstring>& data);
    void SetBinaryValue(const std::wstring& valueName, const std::vector<BYTE>& data);
    void SetBinaryValue(const std::wstring& valueName, const void* data, DWORD dataSize);

    [[nodiscard]] RegResult TrySetDwordValue(const std::wstring& valueName, DWORD data) noexcept;

    [[nodiscard]] RegResult TrySetQwordValue(const std::wstring& valueName,
                                             const ULONGLONG& data) noexcept;

    [[nodiscard]] RegResult TrySetStringValue(const std::wstring& valueName,
                                              const std::wstring& data);

    [[nodiscard]] RegResult TrySetExpandStringValue(const std::wstring& valueName,
                                                    const std::wstring& data);

    [[nodiscard]] RegResult TrySetMultiStringValue(const std::wstring& valueName,
                                                   const std::vector<std::wstring>& data);

    [[nodiscard]] RegResult TrySetBinaryValue(const std::wstring& valueName,
                                              const std::vector<BYTE>& data);

    [[nodiscard]] RegResult TrySetBinaryValue(const std::wstring& valueName,
                                              const void* data,
                                              DWORD dataSize) noexcept;

    [[nodiscard]] DWORD GetDwordValue(const std::wstring& valueName) const;
    [[nodiscard]] ULONGLONG GetQwordValue(const std::wstring& valueName) const;
    [[nodiscard]] std::wstring GetStringValue(const std::wstring& valueName) const;

    enum class ExpandStringOption
    {
        DontExpand,
        Expand
    };

    [[nodiscard]] std::wstring GetExpandStringValue(
        const std::wstring& valueName,
        ExpandStringOption expandOption = ExpandStringOption::DontExpand
    ) const;

    [[nodiscard]] std::vector<std::wstring> GetMultiStringValue(const std::wstring& valueName) const;
    [[nodiscard]] std::vector<BYTE> GetBinaryValue(const std::wstring& valueName) const;

    [[nodiscard]] std::vector<BYTE> GetRawValue(const std::wstring& valueName) const;

    [[nodiscard]] RegExpected<DWORD> TryGetDwordValue(const std::wstring& valueName) const;
    [[nodiscard]] RegExpected<ULONGLONG> TryGetQwordValue(const std::wstring& valueName) const;
    [[nodiscard]] RegExpected<std::wstring> TryGetStringValue(const std::wstring& valueName) const;

    [[nodiscard]] RegExpected<std::wstring> TryGetExpandStringValue(
        const std::wstring& valueName,
        ExpandStringOption expandOption = ExpandStringOption::DontExpand
    ) const;

    [[nodiscard]] RegExpected<std::vector<std::wstring>>
            TryGetMultiStringValue(const std::wstring& valueName) const;

    [[nodiscard]] RegExpected<std::vector<BYTE>>
            TryGetBinaryValue(const std::wstring& valueName) const;

    [[nodiscard]] RegExpected<std::vector<BYTE>>
            TryGetRawValue(const std::wstring& valueName) const;

    struct InfoKey
    {
        DWORD    NumberOfSubKeys;
        DWORD    NumberOfValues;
        FILETIME LastWriteTime;

        InfoKey() noexcept
            : NumberOfSubKeys{0}
            , NumberOfValues{0}
        {
            LastWriteTime.dwHighDateTime = LastWriteTime.dwLowDateTime = 0;
        }

        InfoKey(DWORD numberOfSubKeys, DWORD numberOfValues, FILETIME lastWriteTime) noexcept
            : NumberOfSubKeys{ numberOfSubKeys }
            , NumberOfValues{ numberOfValues }
            , LastWriteTime{ lastWriteTime }
        {
        }
    };

    [[nodiscard]] InfoKey QueryInfoKey() const;

    [[nodiscard]] DWORD QueryValueType(const std::wstring& valueName) const;

    enum class KeyReflection
    {
        ReflectionEnabled,
        ReflectionDisabled
    };

    [[nodiscard]] KeyReflection QueryReflectionKey() const;

    [[nodiscard]] std::vector<std::wstring> EnumSubKeys() const;

    [[nodiscard]] std::vector<std::pair<std::wstring, DWORD>> EnumValues() const;

    [[nodiscard]] bool ContainsValue(const std::wstring& valueName) const;

    [[nodiscard]] bool ContainsSubKey(const std::wstring& subKey) const;

    [[nodiscard]] RegExpected<InfoKey> TryQueryInfoKey() const;

    [[nodiscard]] RegExpected<DWORD> TryQueryValueType(const std::wstring& valueName) const;

    [[nodiscard]] RegExpected<KeyReflection> TryQueryReflectionKey() const;

    [[nodiscard]] RegExpected<std::vector<std::wstring>> TryEnumSubKeys() const;

    [[nodiscard]] RegExpected<std::vector<std::pair<std::wstring, DWORD>>> TryEnumValues() const;

    [[nodiscard]] RegExpected<bool> TryContainsValue(const std::wstring& valueName) const;

    [[nodiscard]] RegExpected<bool> TryContainsSubKey(const std::wstring& subKey) const;

    void DeleteValue(const std::wstring& valueName);
    void DeleteKey(const std::wstring& subKey, REGSAM desiredAccess);
    void DeleteTree(const std::wstring& subKey);
    void CopyTree(const std::wstring& sourceSubKey, const RegKey& destKey);
    void FlushKey();
    void LoadKey(const std::wstring& subKey, const std::wstring& filename);
    void SaveKey(const std::wstring& filename, SECURITY_ATTRIBUTES* securityAttributes) const;
    void EnableReflectionKey();
    void DisableReflectionKey();
    void ConnectRegistry(const std::wstring& machineName, HKEY hKeyPredefined);

    [[nodiscard]] RegResult TryDeleteValue(const std::wstring& valueName) noexcept;
    [[nodiscard]] RegResult TryDeleteKey(const std::wstring& subKey, REGSAM desiredAccess) noexcept;
    [[nodiscard]] RegResult TryDeleteTree(const std::wstring& subKey) noexcept;

    [[nodiscard]] RegResult TryCopyTree(const std::wstring& sourceSubKey,
                                        const RegKey& destKey) noexcept;

    [[nodiscard]] RegResult TryFlushKey() noexcept;

    [[nodiscard]] RegResult TryLoadKey(const std::wstring& subKey,
                                       const std::wstring& filename) noexcept;

    [[nodiscard]] RegResult TrySaveKey(const std::wstring& filename,
                                       SECURITY_ATTRIBUTES* securityAttributes) const noexcept;

    [[nodiscard]] RegResult TryEnableReflectionKey() noexcept;
    [[nodiscard]] RegResult TryDisableReflectionKey() noexcept;

    [[nodiscard]] RegResult TryConnectRegistry(const std::wstring& machineName,
                                               HKEY hKeyPredefined) noexcept;

    [[nodiscard]] static std::wstring RegTypeToString(DWORD regType);

private:

    HKEY m_hKey{ nullptr };
};

class RegException
    : public std::system_error
{
public:
    RegException(LSTATUS errorCode, const char* message);
    RegException(LSTATUS errorCode, const std::string& message);
};

class RegResult
{
public:

    RegResult() noexcept = default;

    explicit RegResult(LSTATUS result) noexcept;

    [[nodiscard]] bool IsOk() const noexcept;

    [[nodiscard]] bool Failed() const noexcept;

    [[nodiscard]] explicit operator bool() const noexcept;

    [[nodiscard]] LSTATUS Code() const noexcept;

    [[nodiscard]] std::wstring ErrorMessage() const;

    [[nodiscard]] std::wstring ErrorMessage(DWORD languageId) const;

private:

    LSTATUS m_result{ ERROR_SUCCESS };
};

template <typename T>
class RegExpected
{
public:

    explicit RegExpected(const RegResult& errorCode) noexcept;

    explicit RegExpected(const T& value);

    explicit RegExpected(T&& value);

    [[nodiscard]] explicit operator bool() const noexcept;

    [[nodiscard]] bool IsValid() const noexcept;

    [[nodiscard]] const T& GetValue() const;

    [[nodiscard]] RegResult GetError() const;

private:

    std::variant<RegResult, T> m_var;
};

inline bool operator==(const RegKey& a, const RegKey& b) noexcept
{
    return a.Get() == b.Get();
}

inline bool operator!=(const RegKey& a, const RegKey& b) noexcept
{
    return a.Get() != b.Get();
}

inline bool operator<(const RegKey& a, const RegKey& b) noexcept
{
    return a.Get() < b.Get();
}

inline bool operator<=(const RegKey& a, const RegKey& b) noexcept
{
    return a.Get() <= b.Get();
}

inline bool operator>(const RegKey& a, const RegKey& b) noexcept
{
    return a.Get() > b.Get();
}

inline bool operator>=(const RegKey& a, const RegKey& b) noexcept
{
    return a.Get() >= b.Get();
}

namespace details
{

template <typename T>
class ScopedLocalFree
{
public:

    typedef T  Type;
    typedef T* TypePtr;

    ScopedLocalFree() noexcept = default;

    ~ScopedLocalFree() noexcept
    {
        Free();
    }

    ScopedLocalFree(const ScopedLocalFree&) = delete;
    ScopedLocalFree(ScopedLocalFree&&) = delete;
    ScopedLocalFree& operator=(const ScopedLocalFree&) = delete;
    ScopedLocalFree& operator=(ScopedLocalFree&&) = delete;

    [[nodiscard]] T* Get() const noexcept
    {
        return m_ptr;
    }

    [[nodiscard]] T** AddressOf() noexcept
    {
        return &m_ptr;
    }

    explicit operator bool() const noexcept
    {
        return (m_ptr != nullptr);
    }

    void Free() noexcept
    {
        if (m_ptr != nullptr)
        {
            ::LocalFree(m_ptr);
            m_ptr = nullptr;
        }
    }

private:
    T* m_ptr{ nullptr };
};

[[nodiscard]] inline std::vector<wchar_t> BuildMultiString(const std::vector<std::wstring>& data)
{

    if (data.empty())
    {

        return std::vector<wchar_t>(2, L'\0');
    }

    size_t totalLen = 0;
    for (const auto& s : data)
    {

        totalLen += (s.length() + 1);
    }

    totalLen++;

    std::vector<wchar_t> multiString;

    multiString.reserve(totalLen);

    for (const auto& s : data)
    {
        if (!s.empty())
        {

            multiString.insert(multiString.end(), s.begin(), s.end());
        }

        multiString.emplace_back(L'\0');
    }

    multiString.emplace_back(L'\0');

    return multiString;
}

[[nodiscard]] inline bool IsDoubleNullTerminated(const std::vector<wchar_t>& data)
{

    if (data.size() < 2)
    {
        return false;
    }

    const size_t lastPosition = data.size() - 1;
    return ((data[lastPosition]     == L'\0')  &&
            (data[lastPosition - 1] == L'\0')) ? true : false;
}

[[nodiscard]] inline std::vector<std::wstring> ParseMultiString(const std::vector<wchar_t>& data)
{

    if (!IsDoubleNullTerminated(data))
    {
        throw RegException{ ERROR_INVALID_DATA, "Not a double-null terminated string." };
    }

    std::vector<std::wstring> result;

    const wchar_t* currStringPtr = data.data();
    const wchar_t* const endPtr  = data.data() + data.size() - 1;

    while (currStringPtr < endPtr)
    {

        const size_t currStringLength = wcslen(currStringPtr);

        if (currStringLength > 0)
        {
            result.emplace_back(currStringPtr, currStringLength);
        }
        else
        {

            result.emplace_back(std::wstring{});
        }

        currStringPtr += currStringLength + 1;
    }

    return result;
}

template <typename T>
[[nodiscard]] inline RegExpected<T> MakeRegExpectedWithError(const LSTATUS retCode)
{
    return RegExpected<T>{ RegResult{ retCode } };
}

[[nodiscard]] inline bool SizeToDwordCastIsSafe([[maybe_unused]] const size_t size) noexcept
{
#ifdef _WIN64

    using DestinationType = DWORD;

    constexpr size_t kMaxDwordValue = static_cast<size_t>((std::numeric_limits<DestinationType>::max)());

    if (size > kMaxDwordValue)
    {

        return false;
    }

    return true;

#else

    static_assert(sizeof(size_t) == sizeof(DWORD));

    return true;

#endif
}

[[nodiscard]] inline DWORD SafeCastSizeToDword(const size_t size)
{

#ifdef _WIN64

    using DestinationType = DWORD;

    if (!SizeToDwordCastIsSafe(size))
    {
        throw std::overflow_error(
            "Input size_t value is too big: size_t value doesn't fit into a DWORD.");
    }

    return static_cast<DestinationType>(size);

#else

    _ASSERTE(SizeToDwordCastIsSafe(size));

    static_assert(sizeof(size_t) == sizeof(DWORD));
    return static_cast<DWORD>(size);

#endif
}

}

inline RegKey::RegKey(const HKEY hKey) noexcept
    : m_hKey{ hKey }
{
}

inline RegKey::RegKey(const HKEY hKeyParent, const std::wstring& subKey)
{
    Create(hKeyParent, subKey);
}

inline RegKey::RegKey(const HKEY hKeyParent, const std::wstring& subKey, const REGSAM desiredAccess)
{
    Create(hKeyParent, subKey, desiredAccess);
}

inline RegKey::RegKey(RegKey&& other) noexcept
    : m_hKey{ other.m_hKey }
{

    other.m_hKey = nullptr;
}

inline RegKey& RegKey::operator=(RegKey&& other) noexcept
{

    if ((this != &other) && (m_hKey != other.m_hKey))
    {

        Close();

        m_hKey = other.m_hKey;
        other.m_hKey = nullptr;
    }
    return *this;
}

inline RegKey::~RegKey() noexcept
{

    Close();
}

inline HKEY RegKey::Get() const noexcept
{
    return m_hKey;
}

inline void RegKey::Close() noexcept
{
    if (IsValid())
    {

        if (! IsPredefined())
        {
            ::RegCloseKey(m_hKey);
        }

        m_hKey = nullptr;
    }
}

inline bool RegKey::IsValid() const noexcept
{
    return m_hKey != nullptr;
}

inline RegKey::operator bool() const noexcept
{
    return IsValid();
}

inline bool RegKey::IsPredefined() const noexcept
{

    if (   (m_hKey == HKEY_CURRENT_USER)
        || (m_hKey == HKEY_LOCAL_MACHINE)
        || (m_hKey == HKEY_CLASSES_ROOT)
        || (m_hKey == HKEY_CURRENT_CONFIG)
        || (m_hKey == HKEY_CURRENT_USER_LOCAL_SETTINGS)
        || (m_hKey == HKEY_PERFORMANCE_DATA)
        || (m_hKey == HKEY_PERFORMANCE_NLSTEXT)
        || (m_hKey == HKEY_PERFORMANCE_TEXT)
        || (m_hKey == HKEY_USERS))
    {
        return true;
    }

    return false;
}

inline HKEY RegKey::Detach() noexcept
{
    HKEY hKey = m_hKey;

    m_hKey = nullptr;

    return hKey;
}

inline void RegKey::Attach(const HKEY hKey) noexcept
{

    if (m_hKey != hKey)
    {

        Close();

        m_hKey = hKey;
    }
}

inline void RegKey::SwapWith(RegKey& other) noexcept
{

    using std::swap;

    swap(m_hKey, other.m_hKey);
}

inline void swap(RegKey& a, RegKey& b) noexcept
{
    a.SwapWith(b);
}

inline void RegKey::Create(
    const HKEY                  hKeyParent,
    const std::wstring&         subKey,
    const REGSAM                desiredAccess
)
{
    constexpr DWORD kDefaultOptions = REG_OPTION_NON_VOLATILE;

    Create(hKeyParent, subKey, desiredAccess, kDefaultOptions,
        nullptr,
        nullptr
    );
}

inline void RegKey::Create(
    const HKEY                  hKeyParent,
    const std::wstring&         subKey,
    const REGSAM                desiredAccess,
    const DWORD                 options,
    SECURITY_ATTRIBUTES* const  securityAttributes,
    DWORD* const                disposition
)
{
    HKEY hKey = nullptr;
    LSTATUS retCode = ::RegCreateKeyExW(
        hKeyParent,
        subKey.c_str(),
        0,
        REG_NONE,
        options,
        desiredAccess,
        securityAttributes,
        &hKey,
        disposition
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegCreateKeyExW failed." };
    }

    Close();

    m_hKey = hKey;
}

inline void RegKey::Open(
    const HKEY              hKeyParent,
    const std::wstring&     subKey,
    const REGSAM            desiredAccess
)
{
    HKEY hKey = nullptr;
    LSTATUS retCode = ::RegOpenKeyExW(
        hKeyParent,
        subKey.c_str(),
        REG_NONE,
        desiredAccess,
        &hKey
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegOpenKeyExW failed." };
    }

    Close();

    m_hKey = hKey;
}

inline RegResult RegKey::TryCreate(
    const HKEY          hKeyParent,
    const std::wstring& subKey,
    const REGSAM        desiredAccess
) noexcept
{
    constexpr DWORD kDefaultOptions = REG_OPTION_NON_VOLATILE;

    return TryCreate(hKeyParent, subKey, desiredAccess, kDefaultOptions,
        nullptr,
        nullptr
    );
}

inline RegResult RegKey::TryCreate(
    const HKEY                  hKeyParent,
    const std::wstring&         subKey,
    const REGSAM                desiredAccess,
    const DWORD                 options,
    SECURITY_ATTRIBUTES* const  securityAttributes,
    DWORD* const                disposition
) noexcept
{
    HKEY hKey = nullptr;
    RegResult retCode{ ::RegCreateKeyExW(
        hKeyParent,
        subKey.c_str(),
        0,
        REG_NONE,
        options,
        desiredAccess,
        securityAttributes,
        &hKey,
        disposition
    ) };
    if (retCode.Failed())
    {
        return retCode;
    }

    Close();

    m_hKey = hKey;

    _ASSERTE(retCode.IsOk());
    return retCode;
}

inline RegResult RegKey::TryOpen(
    const HKEY          hKeyParent,
    const std::wstring& subKey,
    const REGSAM        desiredAccess
) noexcept
{
    HKEY hKey = nullptr;
    RegResult retCode{ ::RegOpenKeyExW(
        hKeyParent,
        subKey.c_str(),
        REG_NONE,
        desiredAccess,
        &hKey
    ) };
    if (retCode.Failed())
    {
        return retCode;
    }

    Close();

    m_hKey = hKey;

    _ASSERTE(retCode.IsOk());
    return retCode;
}

inline void RegKey::SetDwordValue(const std::wstring& valueName, const DWORD data)
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&data),
        sizeof(data)
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot write DWORD value: RegSetValueExW failed." };
    }
}

inline void RegKey::SetQwordValue(const std::wstring& valueName, const ULONGLONG& data)
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_QWORD,
        reinterpret_cast<const BYTE*>(&data),
        sizeof(data)
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot write QWORD value: RegSetValueExW failed." };
    }
}

inline void RegKey::SetStringValue(const std::wstring& valueName, const std::wstring& data)
{
    _ASSERTE(IsValid());

    const DWORD dataSize = details::SafeCastSizeToDword((data.length() + 1) * sizeof(wchar_t));

    LSTATUS retCode = ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(data.c_str()),
        dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot write string value: RegSetValueExW failed." };
    }
}

inline void RegKey::SetExpandStringValue(const std::wstring& valueName, const std::wstring& data)
{
    _ASSERTE(IsValid());

    const DWORD dataSize = details::SafeCastSizeToDword((data.length() + 1) * sizeof(wchar_t));

    LSTATUS retCode = ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_EXPAND_SZ,
        reinterpret_cast<const BYTE*>(data.c_str()),
        dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot write expand string value: RegSetValueExW failed." };
    }
}

inline void RegKey::SetMultiStringValue(
    const std::wstring& valueName,
    const std::vector<std::wstring>& data
)
{
    _ASSERTE(IsValid());

    const std::vector<wchar_t> multiString = details::BuildMultiString(data);

    const DWORD dataSize = details::SafeCastSizeToDword(multiString.size() * sizeof(wchar_t));

    LSTATUS retCode = ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_MULTI_SZ,
        reinterpret_cast<const BYTE*>(multiString.data()),
        dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot write multi-string value: RegSetValueExW failed." };
    }
}

inline void RegKey::SetBinaryValue(const std::wstring& valueName, const std::vector<BYTE>& data)
{
    _ASSERTE(IsValid());

    const DWORD dataSize = details::SafeCastSizeToDword(data.size());

    LSTATUS retCode = ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_BINARY,
        data.data(),
        dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot write binary data value: RegSetValueExW failed." };
    }
}

inline void RegKey::SetBinaryValue(
    const std::wstring& valueName,
    const void* const data,
    const DWORD dataSize
)
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_BINARY,
        static_cast<const BYTE*>(data),
        dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot write binary data value: RegSetValueExW failed." };
    }
}

inline RegResult RegKey::TrySetDwordValue(const std::wstring& valueName, const DWORD data) noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&data),
        sizeof(data)
    ) };
}

inline RegResult RegKey::TrySetQwordValue(const std::wstring& valueName,
                                          const ULONGLONG& data) noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_QWORD,
        reinterpret_cast<const BYTE*>(&data),
        sizeof(data)
    ) };
}

inline RegResult RegKey::TrySetStringValue(const std::wstring& valueName,
                                           const std::wstring& data)
{
    _ASSERTE(IsValid());

    const DWORD dataSize = details::SafeCastSizeToDword((data.length() + 1) * sizeof(wchar_t));

    return RegResult{ ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(data.c_str()),
        dataSize
    ) };
}

inline RegResult RegKey::TrySetExpandStringValue(const std::wstring& valueName,
                                                 const std::wstring& data)
{
    _ASSERTE(IsValid());

    const DWORD dataSize = details::SafeCastSizeToDword((data.length() + 1) * sizeof(wchar_t));

    return RegResult{ ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_EXPAND_SZ,
        reinterpret_cast<const BYTE*>(data.c_str()),
        dataSize
    ) };
}

inline RegResult RegKey::TrySetMultiStringValue(const std::wstring& valueName,
                                                const std::vector<std::wstring>& data)
{
    _ASSERTE(IsValid());

    const std::vector<wchar_t> multiString = details::BuildMultiString(data);

    const DWORD dataSize = details::SafeCastSizeToDword(multiString.size() * sizeof(wchar_t));

    return RegResult{ ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_MULTI_SZ,
        reinterpret_cast<const BYTE*>(multiString.data()),
        dataSize
    ) };
}

inline RegResult RegKey::TrySetBinaryValue(const std::wstring& valueName,
                                           const std::vector<BYTE>& data)
{
    _ASSERTE(IsValid());

    const DWORD dataSize = details::SafeCastSizeToDword(data.size());

    return RegResult{ ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_BINARY,
        data.data(),
        dataSize
    ) };
}

inline RegResult RegKey::TrySetBinaryValue(const std::wstring& valueName,
                                           const void* const data,
                                           const DWORD dataSize) noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegSetValueExW(
        m_hKey,
        valueName.c_str(),
        0,
        REG_BINARY,
        static_cast<const BYTE*>(data),
        dataSize
    ) };
}

inline DWORD RegKey::GetDwordValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    DWORD data = 0;
    DWORD dataSize = sizeof(data);

    constexpr DWORD flags = RRF_RT_REG_DWORD;
    LSTATUS retCode = ::RegGetValueW(
        m_hKey,
        nullptr,
        valueName.c_str(),
        flags,
        nullptr,
        &data,
        &dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot get DWORD value: RegGetValueW failed." };
    }

    return data;
}

inline ULONGLONG RegKey::GetQwordValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    ULONGLONG data = 0;
    DWORD dataSize = sizeof(data);

    constexpr DWORD flags = RRF_RT_REG_QWORD;
    LSTATUS retCode = ::RegGetValueW(
        m_hKey,
        nullptr,
        valueName.c_str(),
        flags,
        nullptr,
        &data,
        &dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot get QWORD value: RegGetValueW failed." };
    }

    return data;
}

inline std::wstring RegKey::GetStringValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    std::wstring result;
    DWORD dataSize = 0;

    constexpr DWORD flags = RRF_RT_REG_SZ;

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            throw RegException{ retCode, "Cannot get the size of the string value: RegGetValueW failed." };
        }

        result.resize(dataSize / sizeof(wchar_t));

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            result.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot get the string value: RegGetValueW failed." };
    }

    result.resize((dataSize / sizeof(wchar_t)) - 1);

    return result;
}

inline std::wstring RegKey::GetExpandStringValue(
    const std::wstring& valueName,
    const ExpandStringOption expandOption
) const
{
    _ASSERTE(IsValid());

    std::wstring result;
    DWORD dataSize = 0;

    DWORD flags = RRF_RT_REG_EXPAND_SZ;

    if (expandOption == ExpandStringOption::DontExpand)
    {
        flags |= RRF_NOEXPAND;
    }

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            throw RegException{ retCode,
                                "Cannot get the size of the expand string value: RegGetValueW failed." };
        }

        result.resize(dataSize / sizeof(wchar_t));

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            result.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot get the expand string value: RegGetValueW failed." };
    }

    result.resize((dataSize / sizeof(wchar_t)) - 1);

    return result;
}

inline std::vector<std::wstring> RegKey::GetMultiStringValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    std::vector<wchar_t> multiString;

    DWORD dataSize = 0;

    constexpr DWORD flags = RRF_RT_REG_MULTI_SZ;

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            throw RegException{ retCode,
                                "Cannot get the size of the multi-string value: RegGetValueW failed." };
        }

        multiString.resize(dataSize / sizeof(wchar_t));

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            multiString.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot get the multi-string value: RegGetValueW failed." };
    }

    multiString.resize(dataSize / sizeof(wchar_t));

    return details::ParseMultiString(multiString);
}

inline std::vector<BYTE> RegKey::GetBinaryValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    std::vector<BYTE> binaryData;

    DWORD dataSize = 0;

    constexpr DWORD flags = RRF_RT_REG_BINARY;

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            throw RegException{ retCode,
                                "Cannot get the size of the binary data: RegGetValueW failed." };
        }

        binaryData.resize(dataSize);

        if (dataSize == 0)
        {
            _ASSERTE(binaryData.empty());
            return binaryData;
        }

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            binaryData.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot get the binary data: RegGetValueW failed." };
    }

    binaryData.resize(dataSize);

    return binaryData;
}

inline std::vector<BYTE> RegKey::GetRawValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    std::vector<BYTE> binaryData;

    DWORD dataSize = 0;

    constexpr DWORD flags = RRF_RT_ANY;

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            throw RegException{ retCode,
                                "Cannot get the size of the raw binary data: RegGetValueW failed." };
        }

        binaryData.resize(dataSize);

        if (dataSize == 0)
        {
            _ASSERTE(binaryData.empty());
            return binaryData;
        }

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            binaryData.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot get the raw binary data: RegGetValueW failed." };
    }

    binaryData.resize(dataSize);

    return binaryData;
}

inline RegExpected<DWORD> RegKey::TryGetDwordValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    using RegValueType = DWORD;

    DWORD data = 0;
    DWORD dataSize = sizeof(data);

    constexpr DWORD flags = RRF_RT_REG_DWORD;
    LSTATUS retCode = ::RegGetValueW(
        m_hKey,
        nullptr,
        valueName.c_str(),
        flags,
        nullptr,
        &data,
        &dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<RegValueType>(retCode);
    }

    return RegExpected<RegValueType>{ data };
}

inline RegExpected<ULONGLONG> RegKey::TryGetQwordValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    using RegValueType = ULONGLONG;

    ULONGLONG data = 0;
    DWORD dataSize = sizeof(data);

    constexpr DWORD flags = RRF_RT_REG_QWORD;
    LSTATUS retCode = ::RegGetValueW(
        m_hKey,
        nullptr,
        valueName.c_str(),
        flags,
        nullptr,
        &data,
        &dataSize
    );
    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<RegValueType>(retCode);
    }

    return RegExpected<RegValueType>{ data };
}

inline RegExpected<std::wstring> RegKey::TryGetStringValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    using RegValueType = std::wstring;

    constexpr DWORD flags = RRF_RT_REG_SZ;

    std::wstring result;

    DWORD dataSize = 0;

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            return details::MakeRegExpectedWithError<RegValueType>(retCode);
        }

        result.resize(dataSize / sizeof(wchar_t));

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            result.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<RegValueType>(retCode);
    }

    result.resize((dataSize / sizeof(wchar_t)) - 1);

    return RegExpected<RegValueType>{ result };
}

inline RegExpected<std::wstring> RegKey::TryGetExpandStringValue(
    const std::wstring& valueName,
    const ExpandStringOption expandOption
) const
{
    _ASSERTE(IsValid());

    using RegValueType = std::wstring;

    DWORD flags = RRF_RT_REG_EXPAND_SZ;

    if (expandOption == ExpandStringOption::DontExpand)
    {
        flags |= RRF_NOEXPAND;
    }

    std::wstring result;
    DWORD dataSize = 0;
    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            return details::MakeRegExpectedWithError<RegValueType>(retCode);
        }

        result.resize(dataSize / sizeof(wchar_t));

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            result.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<RegValueType>(retCode);
    }

    result.resize((dataSize / sizeof(wchar_t)) - 1);

    return RegExpected<RegValueType>{ result };
}

inline RegExpected<std::vector<std::wstring>>
    RegKey::TryGetMultiStringValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    using RegValueType = std::vector<std::wstring>;

    constexpr DWORD flags = RRF_RT_REG_MULTI_SZ;

    std::vector<wchar_t> data;

    DWORD dataSize = 0;

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            return details::MakeRegExpectedWithError<RegValueType>(retCode);
        }

        data.resize(dataSize / sizeof(wchar_t));

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            data.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<RegValueType>(retCode);
    }

    data.resize(dataSize / sizeof(wchar_t));

    return RegExpected<RegValueType>{ details::ParseMultiString(data) };
}

inline RegExpected<std::vector<BYTE>>
    RegKey::TryGetBinaryValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    using RegValueType = std::vector<BYTE>;

    constexpr DWORD flags = RRF_RT_REG_BINARY;

    std::vector<BYTE> data;

    DWORD dataSize = 0;

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            return details::MakeRegExpectedWithError<RegValueType>(retCode);
        }

        data.resize(dataSize);

        if (dataSize == 0)
        {
            _ASSERTE(data.empty());
            return RegExpected<RegValueType>{ data };
        }

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            data.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<RegValueType>(retCode);
    }

    data.resize(dataSize);

    return RegExpected<RegValueType>{ data };
}

inline RegExpected<std::vector<BYTE>>
    RegKey::TryGetRawValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    using RegValueType = std::vector<BYTE>;

    constexpr DWORD flags = RRF_RT_ANY;

    std::vector<BYTE> data;

    DWORD dataSize = 0;

    LSTATUS retCode = ERROR_MORE_DATA;

    while (retCode == ERROR_MORE_DATA)
    {

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            nullptr,
            &dataSize
        );
        if (retCode != ERROR_SUCCESS)
        {
            return details::MakeRegExpectedWithError<RegValueType>(retCode);
        }

        data.resize(dataSize);

        if (dataSize == 0)
        {
            _ASSERTE(data.empty());
            return RegExpected<RegValueType>{ data };
        }

        retCode = ::RegGetValueW(
            m_hKey,
            nullptr,
            valueName.c_str(),
            flags,
            nullptr,
            data.data(),
            &dataSize
        );
    }

    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<RegValueType>(retCode);
    }

    data.resize(dataSize);

    return RegExpected<RegValueType>{ data };
}

inline std::vector<std::wstring> RegKey::EnumSubKeys() const
{
    _ASSERTE(IsValid());

    DWORD subKeyCount = 0;
    DWORD maxSubKeyNameLen = 0;
    LSTATUS retCode = ::RegQueryInfoKeyW(
        m_hKey,
        nullptr,
        nullptr,
        nullptr,
        &subKeyCount,
        &maxSubKeyNameLen,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{
            retCode,
            "RegQueryInfoKeyW failed while preparing for subkey enumeration."
        };
    }

    maxSubKeyNameLen++;

    auto nameBuffer = std::make_unique<wchar_t[]>(maxSubKeyNameLen);

    std::vector<std::wstring> subkeyNames;

    subkeyNames.reserve(subKeyCount);

    for (DWORD index = 0; index < subKeyCount; index++)
    {

        DWORD subKeyNameLen = maxSubKeyNameLen;
        retCode = ::RegEnumKeyExW(
            m_hKey,
            index,
            nameBuffer.get(),
            &subKeyNameLen,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        );
        if (retCode != ERROR_SUCCESS)
        {
            throw RegException{ retCode, "Cannot enumerate subkeys: RegEnumKeyExW failed." };
        }

        subkeyNames.emplace_back(nameBuffer.get(), subKeyNameLen);
    }

    return subkeyNames;
}

inline std::vector<std::pair<std::wstring, DWORD>> RegKey::EnumValues() const
{
    _ASSERTE(IsValid());

    DWORD valueCount = 0;
    DWORD maxValueNameLen = 0;
    LSTATUS retCode = ::RegQueryInfoKeyW(
        m_hKey,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &valueCount,
        &maxValueNameLen,
        nullptr,
        nullptr,
        nullptr
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{
            retCode,
            "RegQueryInfoKeyW failed while preparing for value enumeration."
        };
    }

    maxValueNameLen++;

    auto nameBuffer = std::make_unique<wchar_t[]>(maxValueNameLen);

    std::vector<std::pair<std::wstring, DWORD>> valueInfo;

    valueInfo.reserve(valueCount);

    for (DWORD index = 0; index < valueCount; index++)
    {

        DWORD valueNameLen = maxValueNameLen;
        DWORD valueType = 0;
        retCode = ::RegEnumValueW(
            m_hKey,
            index,
            nameBuffer.get(),
            &valueNameLen,
            nullptr,
            &valueType,
            nullptr,
            nullptr
        );
        if (retCode != ERROR_SUCCESS)
        {
            throw RegException{ retCode, "Cannot enumerate values: RegEnumValueW failed." };
        }

        valueInfo.emplace_back(
            std::wstring{ nameBuffer.get(), valueNameLen },
            valueType
        );
    }

    return valueInfo;
}

inline bool RegKey::ContainsValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegGetValueW(
        m_hKey,
        nullptr,
        valueName.c_str(),
        RRF_RT_ANY,
        nullptr,
        nullptr, nullptr
    );
    if (retCode == ERROR_SUCCESS)
    {

        return true;
    }
    else if (retCode == ERROR_FILE_NOT_FOUND)
    {

        return false;
    }
    else
    {

        throw RegException{
            retCode,
            "RegGetValueW failed when checking if the current key contains the specified value."
        };
    }
}

inline bool RegKey::ContainsSubKey(const std::wstring& subKey) const
{
    _ASSERTE(IsValid());

    HKEY hSubKey = nullptr;
    LSTATUS retCode = ::RegOpenKeyExW(
        m_hKey,
        subKey.c_str(),
        0,
        KEY_READ,
        &hSubKey
    );
    if (retCode == ERROR_SUCCESS)
    {

        ::RegCloseKey(hSubKey);
        hSubKey = nullptr;

        return true;
    }
    else if ((retCode == ERROR_FILE_NOT_FOUND) || (retCode == ERROR_PATH_NOT_FOUND))
    {

        return false;
    }
    else
    {

        throw RegException{
            retCode,
            "RegOpenKeyExW failed when checking if the current key contains the specified sub-key."
        };
    }
}

inline RegExpected<std::vector<std::wstring>> RegKey::TryEnumSubKeys() const
{
    _ASSERTE(IsValid());

    using ReturnType = std::vector<std::wstring>;

    DWORD subKeyCount = 0;
    DWORD maxSubKeyNameLen = 0;
    LSTATUS retCode = ::RegQueryInfoKeyW(
        m_hKey,
        nullptr,
        nullptr,
        nullptr,
        &subKeyCount,
        &maxSubKeyNameLen,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );
    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<ReturnType>(retCode);
    }

    maxSubKeyNameLen++;

    auto nameBuffer = std::make_unique<wchar_t[]>(maxSubKeyNameLen);

    std::vector<std::wstring> subkeyNames;

    subkeyNames.reserve(subKeyCount);

    for (DWORD index = 0; index < subKeyCount; index++)
    {

        DWORD subKeyNameLen = maxSubKeyNameLen;
        retCode = ::RegEnumKeyExW(
            m_hKey,
            index,
            nameBuffer.get(),
            &subKeyNameLen,
            nullptr,
            nullptr,
            nullptr,
            nullptr
        );
        if (retCode != ERROR_SUCCESS)
        {
            return details::MakeRegExpectedWithError<ReturnType>(retCode);
        }

        subkeyNames.emplace_back(nameBuffer.get(), subKeyNameLen);
    }

    return RegExpected<ReturnType>{ subkeyNames };
}

inline RegExpected<std::vector<std::pair<std::wstring, DWORD>>> RegKey::TryEnumValues() const
{
    _ASSERTE(IsValid());

    using ReturnType = std::vector<std::pair<std::wstring, DWORD>>;

    DWORD valueCount = 0;
    DWORD maxValueNameLen = 0;
    LSTATUS retCode = ::RegQueryInfoKeyW(
        m_hKey,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &valueCount,
        &maxValueNameLen,
        nullptr,
        nullptr,
        nullptr
    );
    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<ReturnType>(retCode);
    }

    maxValueNameLen++;

    auto nameBuffer = std::make_unique<wchar_t[]>(maxValueNameLen);

    std::vector<std::pair<std::wstring, DWORD>> valueInfo;

    valueInfo.reserve(valueCount);

    for (DWORD index = 0; index < valueCount; index++)
    {

        DWORD valueNameLen = maxValueNameLen;
        DWORD valueType = 0;
        retCode = ::RegEnumValueW(
            m_hKey,
            index,
            nameBuffer.get(),
            &valueNameLen,
            nullptr,
            &valueType,
            nullptr,
            nullptr
        );
        if (retCode != ERROR_SUCCESS)
        {
            return details::MakeRegExpectedWithError<ReturnType>(retCode);
        }

        valueInfo.emplace_back(
            std::wstring{ nameBuffer.get(), valueNameLen },
            valueType
        );
    }

    return RegExpected<ReturnType>{ valueInfo };
}

inline RegExpected<bool> RegKey::TryContainsValue(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegGetValueW(
        m_hKey,
        nullptr,
        valueName.c_str(),
        RRF_RT_ANY,
        nullptr,
        nullptr, nullptr
    );
    if (retCode == ERROR_SUCCESS)
    {

        return RegExpected<bool>{ true };
    }
    else if (retCode == ERROR_FILE_NOT_FOUND)
    {

        return RegExpected<bool>{ false };
    }
    else
    {

        return details::MakeRegExpectedWithError<bool>(retCode);
    }
}

inline RegExpected<bool> RegKey::TryContainsSubKey(const std::wstring& subKey) const
{
    _ASSERTE(IsValid());

    HKEY hSubKey = nullptr;
    LSTATUS retCode = ::RegOpenKeyExW(
        m_hKey,
        subKey.c_str(),
        0,
        KEY_READ,
        &hSubKey
    );
    if (retCode == ERROR_SUCCESS)
    {

        ::RegCloseKey(hSubKey);
        hSubKey = nullptr;

        return RegExpected<bool>{ true };
    }
    else if ((retCode == ERROR_FILE_NOT_FOUND) || (retCode == ERROR_PATH_NOT_FOUND))
    {

        return RegExpected<bool>{ false };
    }
    else
    {

        return details::MakeRegExpectedWithError<bool>(retCode);
    }
}

inline DWORD RegKey::QueryValueType(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    DWORD typeId = 0;

    LSTATUS retCode = ::RegQueryValueExW(
        m_hKey,
        valueName.c_str(),
        nullptr,
        &typeId,
        nullptr,
        nullptr
    );

    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "Cannot get the value type: RegQueryValueExW failed." };
    }

    return typeId;
}

inline RegExpected<DWORD> RegKey::TryQueryValueType(const std::wstring& valueName) const
{
    _ASSERTE(IsValid());

    using ReturnType = DWORD;

    DWORD typeId = 0;

    LSTATUS retCode = ::RegQueryValueExW(
        m_hKey,
        valueName.c_str(),
        nullptr,
        &typeId,
        nullptr,
        nullptr
    );

    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<ReturnType>(retCode);
    }

    return RegExpected<ReturnType>{ typeId };
}

inline RegKey::InfoKey RegKey::QueryInfoKey() const
{
    _ASSERTE(IsValid());

    InfoKey infoKey{};
    LSTATUS retCode = ::RegQueryInfoKeyW(
        m_hKey,
        nullptr,
        nullptr,
        nullptr,
        &(infoKey.NumberOfSubKeys),
        nullptr,
        nullptr,
        &(infoKey.NumberOfValues),
        nullptr,
        nullptr,
        nullptr,
        &(infoKey.LastWriteTime)
    );
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegQueryInfoKeyW failed." };
    }

    return infoKey;
}

inline RegExpected<RegKey::InfoKey> RegKey::TryQueryInfoKey() const
{
    _ASSERTE(IsValid());

    using ReturnType = RegKey::InfoKey;

    InfoKey infoKey{};
    LSTATUS retCode = ::RegQueryInfoKeyW(
        m_hKey,
        nullptr,
        nullptr,
        nullptr,
        &(infoKey.NumberOfSubKeys),
        nullptr,
        nullptr,
        &(infoKey.NumberOfValues),
        nullptr,
        nullptr,
        nullptr,
        &(infoKey.LastWriteTime)
    );
    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<ReturnType>(retCode);
    }

    return RegExpected<ReturnType>{ infoKey };
}

inline RegKey::KeyReflection RegKey::QueryReflectionKey() const
{
    BOOL isReflectionDisabled = FALSE;
    LSTATUS retCode = ::RegQueryReflectionKey(m_hKey, &isReflectionDisabled);
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegQueryReflectionKey failed." };
    }

    return (isReflectionDisabled ? KeyReflection::ReflectionDisabled
                                 : KeyReflection::ReflectionEnabled);
}

inline RegExpected<RegKey::KeyReflection> RegKey::TryQueryReflectionKey() const
{
    using ReturnType = RegKey::KeyReflection;

    BOOL isReflectionDisabled = FALSE;
    LSTATUS retCode = ::RegQueryReflectionKey(m_hKey, &isReflectionDisabled);
    if (retCode != ERROR_SUCCESS)
    {
        return details::MakeRegExpectedWithError<ReturnType>(retCode);
    }

    KeyReflection keyReflection = isReflectionDisabled ? KeyReflection::ReflectionDisabled
                                                       : KeyReflection::ReflectionEnabled;
    return RegExpected<ReturnType>{ keyReflection };
}

inline void RegKey::DeleteValue(const std::wstring& valueName)
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegDeleteValueW(m_hKey, valueName.c_str());
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegDeleteValueW failed." };
    }
}

inline RegResult RegKey::TryDeleteValue(const std::wstring& valueName) noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegDeleteValueW(m_hKey, valueName.c_str()) };
}

inline void RegKey::DeleteKey(const std::wstring& subKey, const REGSAM desiredAccess)
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegDeleteKeyExW(m_hKey, subKey.c_str(), desiredAccess, 0);
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegDeleteKeyExW failed." };
    }
}

inline RegResult RegKey::TryDeleteKey(const std::wstring& subKey,
                                      const REGSAM desiredAccess) noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegDeleteKeyExW(m_hKey, subKey.c_str(), desiredAccess, 0) };
}

inline void RegKey::DeleteTree(const std::wstring& subKey)
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegDeleteTreeW(m_hKey, subKey.c_str());
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegDeleteTreeW failed." };
    }
}

inline RegResult RegKey::TryDeleteTree(const std::wstring& subKey) noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegDeleteTreeW(m_hKey, subKey.c_str()) };
}

inline void RegKey::CopyTree(const std::wstring& sourceSubKey, const RegKey& destKey)
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegCopyTreeW(m_hKey, sourceSubKey.c_str(), destKey.Get());
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegCopyTreeW failed." };
    }
}

inline RegResult RegKey::TryCopyTree(const std::wstring& sourceSubKey,
                                     const RegKey& destKey) noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegCopyTreeW(m_hKey, sourceSubKey.c_str(), destKey.Get()) };
}

inline void RegKey::FlushKey()
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegFlushKey(m_hKey);
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegFlushKey failed." };
    }
}

inline RegResult RegKey::TryFlushKey() noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegFlushKey(m_hKey) };
}

inline void RegKey::LoadKey(const std::wstring& subKey, const std::wstring& filename)
{
    Close();

    LSTATUS retCode = ::RegLoadKeyW(m_hKey, subKey.c_str(), filename.c_str());
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegLoadKeyW failed." };
    }
}

inline RegResult RegKey::TryLoadKey(const std::wstring& subKey,
                                    const std::wstring& filename) noexcept
{
    Close();

    return RegResult{ ::RegLoadKeyW(m_hKey, subKey.c_str(), filename.c_str()) };
}

inline void RegKey::SaveKey(
    const std::wstring& filename,
    SECURITY_ATTRIBUTES* const securityAttributes
) const
{
    _ASSERTE(IsValid());

    LSTATUS retCode = ::RegSaveKeyW(m_hKey, filename.c_str(), securityAttributes);
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegSaveKeyW failed." };
    }
}

inline RegResult RegKey::TrySaveKey(
    const std::wstring& filename,
    SECURITY_ATTRIBUTES* const securityAttributes
) const noexcept
{
    _ASSERTE(IsValid());

    return RegResult{ ::RegSaveKeyW(m_hKey, filename.c_str(), securityAttributes) };
}

inline void RegKey::EnableReflectionKey()
{
    LSTATUS retCode = ::RegEnableReflectionKey(m_hKey);
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegEnableReflectionKey failed." };
    }
}

inline RegResult RegKey::TryEnableReflectionKey() noexcept
{
    return RegResult{ ::RegEnableReflectionKey(m_hKey) };
}

inline void RegKey::DisableReflectionKey()
{
    LSTATUS retCode = ::RegDisableReflectionKey(m_hKey);
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegDisableReflectionKey failed." };
    }
}

inline RegResult RegKey::TryDisableReflectionKey() noexcept
{
    return RegResult{ ::RegDisableReflectionKey(m_hKey) };
}

inline void RegKey::ConnectRegistry(const std::wstring& machineName, const HKEY hKeyPredefined)
{

    Close();

    HKEY hKeyResult = nullptr;
    LSTATUS retCode = ::RegConnectRegistryW(machineName.c_str(), hKeyPredefined, &hKeyResult);
    if (retCode != ERROR_SUCCESS)
    {
        throw RegException{ retCode, "RegConnectRegistryW failed." };
    }

    m_hKey = hKeyResult;
}

inline RegResult RegKey::TryConnectRegistry(const std::wstring& machineName,
                                            const HKEY hKeyPredefined) noexcept
{

    Close();

    HKEY hKeyResult = nullptr;
    RegResult retCode{ ::RegConnectRegistryW(machineName.c_str(), hKeyPredefined, &hKeyResult) };
    if (retCode.Failed())
    {
        return retCode;
    }

    m_hKey = hKeyResult;

    _ASSERTE(retCode.IsOk());
    return retCode;
}

inline std::wstring RegKey::RegTypeToString(const DWORD regType)
{
    switch (regType)
    {
        case REG_SZ:        return L"REG_SZ";
        case REG_EXPAND_SZ: return L"REG_EXPAND_SZ";
        case REG_MULTI_SZ:  return L"REG_MULTI_SZ";
        case REG_DWORD:     return L"REG_DWORD";
        case REG_QWORD:     return L"REG_QWORD";
        case REG_BINARY:    return L"REG_BINARY";

        default:            return L"Unknown/unsupported registry type";
    }
}

inline RegException::RegException(const LSTATUS errorCode, const char* const message)
    : std::system_error{ errorCode, std::system_category(), message }
{}

inline RegException::RegException(const LSTATUS errorCode, const std::string& message)
    : std::system_error{ errorCode, std::system_category(), message }
{}

inline RegResult::RegResult(const LSTATUS result) noexcept
    : m_result{ result }
{}

inline bool RegResult::IsOk() const noexcept
{
    return m_result == ERROR_SUCCESS;
}

inline bool RegResult::Failed() const noexcept
{
    return m_result != ERROR_SUCCESS;
}

inline RegResult::operator bool() const noexcept
{
    return IsOk();
}

inline LSTATUS RegResult::Code() const noexcept
{
    return m_result;
}

inline std::wstring RegResult::ErrorMessage() const
{
    return ErrorMessage(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
}

inline std::wstring RegResult::ErrorMessage(const DWORD languageId) const
{

    details::ScopedLocalFree<wchar_t> messagePtr;
    DWORD retCode = ::FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        m_result,
        languageId,
        reinterpret_cast<LPWSTR>(messagePtr.AddressOf()),
        0,
        nullptr
    );
    if (retCode == 0)
    {

        return std::wstring{};
    }

    return std::wstring{ messagePtr.Get() };
}

template <typename T>
inline RegExpected<T>::RegExpected(const RegResult& errorCode) noexcept
    : m_var{ errorCode }
{}

template <typename T>
inline RegExpected<T>::RegExpected(const T& value)
    : m_var{ value }
{}

template <typename T>
inline RegExpected<T>::RegExpected(T&& value)
    : m_var{ std::move(value) }
{}

template <typename T>
inline RegExpected<T>::operator bool() const noexcept
{
    return IsValid();
}

template <typename T>
inline bool RegExpected<T>::IsValid() const noexcept
{
    return std::holds_alternative<T>(m_var);
}

template <typename T>
inline const T& RegExpected<T>::GetValue() const
{

    _ASSERTE(IsValid());

    return std::get<T>(m_var);
}

template <typename T>
inline RegResult RegExpected<T>::GetError() const
{

    _ASSERTE(!IsValid());

    return std::get<RegResult>(m_var);
}

}

#endif
