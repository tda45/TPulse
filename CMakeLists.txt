cmake_minimum_required(VERSION 3.15)
project(TPulse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Doğrudan MSYS2'nin statik kütüphanesini hedef alıyoruz
set(STATIC_CURL_LIB "C:/msys64/mingw64/lib/libcurl.a")
set(CURL_INCLUDE_DIR "C:/msys64/mingw64/include")

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

add_executable(tpulse ${SOURCES})

# Ön işlemciye curl'ün statik bağlanacağını kesin olarak bildiriyoruz
target_compile_definitions(tpulse PRIVATE CURL_STATICLIB)
target_include_directories(tpulse PRIVATE ${CURL_INCLUDE_DIR})

# iphlpapi kütüphanesini ağ bağımlılıklarının (ws2_32) hemen yanına ekledik
target_link_libraries(tpulse PRIVATE 
    ${STATIC_CURL_LIB}
    ssh2
    nghttp2
    nghttp3
    ngtcp2_crypto_ossl
    ngtcp2
    ssl
    crypto
    psl
    idn2
    unistring
    iconv
    zstd
    brotlidec
    brotlienc
    brotlicommon
    z
    winhttp 
    ws2_32               
    iphlpapi             # if_nametoindex hatasını çözen sihirli değnek
    wldap32 
    crypt32 
    bcrypt 
    secur32
    normaliz
)

# Tamamen bağımsız bir .exe için statik bayrakları
target_link_options(tpulse PRIVATE "-static" "-static-libgcc" "-static-libstdc++")