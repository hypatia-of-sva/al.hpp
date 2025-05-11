/*
 * al.hpp - C++ Wrapper aroung OpenAL. Also initializes core and extension function pointers
 * without putting any symbols except types and #define's into the global namespace.
 * This Version depends on alad.h, al.h, alc.h, alext.h etc. being present and up to date
 * Inspired by vulkan.hpp, Copyright 2015-2023 The Khronos Group Inc.
 *
 *  Copyright (c) 2023, Hypatia of Sva <hypatia dot sva at posteo dot eu>
 *  SPDX-License-Identifier: MIT
 */

#pragma once

#ifndef AL_HPP
#define AL_HPP

#define AL_HPP_REVISION      1L
#define AL_HPP_REVISION_DATE 20230311L


// preroll, copied out of vulkan.hpp (rev. 1.3.242) with changed macro names


#if defined( _MSVC_LANG )
#  define AL_HPP_CPLUSPLUS _MSVC_LANG
#else
#  define AL_HPP_CPLUSPLUS __cplusplus
#endif

#if 201703L < AL_HPP_CPLUSPLUS
#  define AL_HPP_CPP_VERSION 20
#elif 201402L < AL_HPP_CPLUSPLUS
#  define AL_HPP_CPP_VERSION 17
#elif 201103L < AL_HPP_CPLUSPLUS
#  define AL_HPP_CPP_VERSION 14
#elif 199711L < AL_HPP_CPLUSPLUS
#  define AL_HPP_CPP_VERSION 11
#else
#  error "al.hpp needs at least c++ standard version 11"
#endif


// insert standard library includes here
#include <string>
#include <sstream>
#include <vector>
#include <memory>

#if 17 <= AL_HPP_CPP_VERSION
#include <filesystem>
#define AL_HPP_PATH std::filesystem::path
#else
#define AL_HPP_PATH std::string
#endif

#include <gsl/gsl> //for owner

// polyfill for <string_vieW>
// see https://github.com/martinmoene/string-view-lite
#include <nonstd/string_view.hpp>


#if defined( __unix__ ) || defined( __APPLE__ ) || defined( __QNXNTO__ ) || defined( __Fuchsia__ )
#    include <dlfcn.h>
#    define  AL_HPP_DYNAMIC_LOADER_MODULE_TYPE void*
#    define  AL_HPP_DYNAMIC_LOADER_LOAD_FUNCTION(path) dlopen(path,RTLD_LAZY | RTLD_LOCAL)
#    define  AL_HPP_DYNAMIC_LOADER_SYMBOL_FUNCTION dlsym
#    define  AL_HPP_DYNAMIC_LOADER_CLOSE_FUNCTION dlclose

//there are also libopenal.so.1.[X].[Y] and libopenal.1.[X].[Y].dylib respectively, but it would be difficult to look all of those up
#    if defined(__APPLE__)
//not tested myself; the only references I could find are https://github.com/ToweOPrO/sadsad and https://pastebin.com/MEmh3ZFr, which is at least tenuous
#      define AL_HPP_LIBRARY_NAME           "libopenal.1.dylib"
#      define AL_HPP_SECONDARY_LIBRARY_NAME "libopenal.dylib"
#    else
#      define AL_HPP_LIBRARY_NAME           "libopenal.so.1"
#      define AL_HPP_SECONDARY_LIBRARY_NAME "libopenal.so"
#    endif

#elif defined( _WIN32 )
typedef struct HINSTANCE__ * HINSTANCE;
#    if defined( _WIN64 )
typedef int64_t( __stdcall * FARPROC )();
#    else
typedef int( __stdcall * FARPROC )();
#    endif
extern "C" __declspec( dllimport ) HINSTANCE __stdcall LoadLibraryA( char const * lpLibFileName );
extern "C" __declspec( dllimport ) int __stdcall FreeLibrary( HINSTANCE hLibModule );
extern "C" __declspec( dllimport ) FARPROC __stdcall GetProcAddress( HINSTANCE hModule, const char * lpProcName );
#    define  AL_HPP_DYNAMIC_LOADER_MODULE_TYPE HINSTANCE
#    define  AL_HPP_DYNAMIC_LOADER_LOAD_FUNCTION(path) LoadLibraryA(path)
#    define  AL_HPP_DYNAMIC_LOADER_SYMBOL_FUNCTION GetProcAddress
#    define  AL_HPP_DYNAMIC_LOADER_CLOSE_FUNCTION FreeLibrary

#    define AL_HPP_LIBRARY_NAME           "OpenAL32.dll"
#    define AL_HPP_SECONDARY_LIBRARY_NAME "soft_oal.dll"
#endif



#define AL_NO_PROTOTYPES
#define ALC_NO_PROTOTYPES
#include <AL/alext.h>
#include <AL/efx-presets.h>






#if !defined( AL_HPP_NO_EXCEPTIONS )
#  include <system_error>  // std::is_error_code_enum
#endif

#if !defined( AL_HPP_ASSERT )
#  include <cassert>
#  define AL_HPP_ASSERT assert
#endif

#if !defined( AL_HPP_ASSERT_ON_RESULT )
#  define AL_HPP_ASSERT_ON_RESULT AL_HPP_ASSERT
#endif

#if !defined( AL_HPP_STATIC_ASSERT )
#  define AL_HPP_STATIC_ASSERT static_assert
#endif



#if !defined( __has_include )
#  define __has_include( x ) false
#endif

#if ( 201907 <= __cpp_lib_three_way_comparison ) && __has_include( <compare> ) && !defined( AL_HPP_NO_SPACESHIP_OPERATOR )
#  define AL_HPP_HAS_SPACESHIP_OPERATOR
#endif
#if defined(AL_HPP_HAS_SPACESHIP_OPERATOR )
#  include <compare>
#endif

#if ( 201803 <= __cpp_lib_span )
#  define AL_HPP_SUPPORT_SPAN
#  include <span>
#endif



#if !defined( AL_HPP_HAS_UNRESTRICTED_UNIONS )
#  if defined( __clang__ )
#    if __has_feature( cxx_unrestricted_unions )
#      define AL_HPP_HAS_UNRESTRICTED_UNIONS
#    endif
#  elif defined( __GNUC__ )
#    define GCC_VERSION ( __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ )
#    if 40600 <= GCC_VERSION
#      define AL_HPP_HAS_UNRESTRICTED_UNIONS
#    endif
#  elif defined( _MSC_VER )
#    if 1900 <= _MSC_VER
#      define AL_HPP_HAS_UNRESTRICTED_UNIONS
#    endif
#  endif
#endif

#if !defined( AL_HPP_INLINE )
#  if defined( __clang__ )
#    if __has_attribute( always_inline )
#      define AL_HPP_INLINE __attribute__( ( always_inline ) ) __inline__
#    else
#      define AL_HPP_INLINE inline
#    endif
#  elif defined( __GNUC__ )
#    define AL_HPP_INLINE __attribute__( ( always_inline ) ) __inline__
#  elif defined( _MSC_VER )
#    define AL_HPP_INLINE inline
#  else
#    define AL_HPP_INLINE inline
#  endif
#endif

#if defined( AL_HPP_TYPESAFE_CONVERSION )
#  define AL_HPP_TYPESAFE_EXPLICIT
#else
#  define AL_HPP_TYPESAFE_EXPLICIT explicit
#endif

#if defined( __cpp_constexpr )
#  define AL_HPP_CONSTEXPR constexpr
#  if __cpp_constexpr >= 201304
#    define AL_HPP_CONSTEXPR_14 constexpr
#  else
#    define AL_HPP_CONSTEXPR_14
#  endif
#  define AL_HPP_CONST_OR_CONSTEXPR constexpr
#else
#  define AL_HPP_CONSTEXPR
#  define AL_HPP_CONSTEXPR_14
#  define AL_HPP_CONST_OR_CONSTEXPR const
#endif

#if !defined( AL_HPP_NOEXCEPT )
#  if defined( _MSC_VER ) && ( _MSC_VER <= 1800 )
#    define AL_HPP_NOEXCEPT
#  else
#    define AL_HPP_NOEXCEPT     noexcept
#    define AL_HPP_HAS_NOEXCEPT 1
#    if defined( AL_HPP_NO_EXCEPTIONS )
#      define AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS noexcept
#    else
#      define AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS
#    endif
#  endif
#endif

#if defined( AL_HPP_NO_EXCEPTIONS )
#  define AL_HPP_THROW_WHEN_EXCEPTIONS(x)
#else
#  define AL_HPP_THROW_WHEN_EXCEPTIONS(x) throw(x);
#endif



#if 14 <= AL_HPP_CPP_VERSION
#  define AL_HPP_DEPRECATED( msg ) [[deprecated( msg )]]
#else
#  define AL_HPP_DEPRECATED( msg )
#endif

#if ( 17 <= AL_HPP_CPP_VERSION ) && !defined( AL_HPP_NO_NODISCARD_WARNINGS )
#  define AL_HPP_NODISCARD [[nodiscard]]
#  if defined( AL_HPP_NO_EXCEPTIONS )
#    define AL_HPP_NODISCARD_WHEN_NO_EXCEPTIONS [[nodiscard]]
#  else
#    define AL_HPP_NODISCARD_WHEN_NO_EXCEPTIONS
#  endif
#else
#  define AL_HPP_NODISCARD
#  define AL_HPP_NODISCARD_WHEN_NO_EXCEPTIONS
#endif






// function pointer declarations
namespace alc {
    // Core ALC function pointers
    LPALCCREATECONTEXT               createContext               = nullptr;
    LPALCMAKECONTEXTCURRENT          makeContextCurrent          = nullptr;
    LPALCPROCESSCONTEXT              processContext              = nullptr;
    LPALCSUSPENDCONTEXT              suspendContext              = nullptr;
    LPALCDESTROYCONTEXT              destroyContext              = nullptr;
    LPALCGETCURRENTCONTEXT           getCurrentContext           = nullptr;
    LPALCGETCONTEXTSDEVICE           getContextsDevice           = nullptr;
    LPALCOPENDEVICE                  openDevice                  = nullptr;
    LPALCCLOSEDEVICE                 closeDevice                 = nullptr;
    LPALCGETERROR                    getError                    = nullptr;
    LPALCISEXTENSIONPRESENT          isExtensionPresent          = nullptr;
    LPALCGETPROCADDRESS              getProcAddress              = nullptr;
    LPALCGETENUMVALUE                getEnumValue                = nullptr;
    LPALCGETSTRING                   getString                   = nullptr;
    LPALCGETINTEGERV                 getIntegerv                 = nullptr;
    LPALCCAPTUREOPENDEVICE           captureOpenDevice           = nullptr;
    LPALCCAPTURECLOSEDEVICE          captureCloseDevice          = nullptr;
    LPALCCAPTURESTART                captureStart                = nullptr;
    LPALCCAPTURESTOP                 captureStop                 = nullptr;
    LPALCCAPTURESAMPLES              captureSamples              = nullptr;

    // ALC extension function pointers
    //ALC_EXT_thread_local_context
    PFNALCSETTHREADCONTEXTPROC       setThreadContext            = nullptr;
    PFNALCGETTHREADCONTEXTPROC       getThreadContext            = nullptr;
    //ALC_SOFT_loopback
    LPALCLOOPBACKOPENDEVICESOFT      loopbackOpenDeviceSOFT      = nullptr;
    LPALCISRENDERFORMATSUPPORTEDSOFT isRenderFormatSupportedSOFT = nullptr;
    LPALCRENDERSAMPLESSOFT           renderSamplesSOFT           = nullptr;
    //ALC_SOFT_pause_device
    LPALCDEVICEPAUSESOFT             devicePauseSOFT             = nullptr;
    LPALCDEVICERESUMESOFT            deviceResumeSOFT            = nullptr;
    //ALC_SOFT_HRTF
    LPALCGETSTRINGISOFT              getStringiSOFT              = nullptr;
    LPALCRESETDEVICESOFT             resetDeviceSOFT             = nullptr;
    //ALC_SOFT_device_clock
    LPALCGETINTEGER64VSOFT           getInteger64vSOFT           = nullptr;
    //ALC_SOFT_reopen_device
    LPALCREOPENDEVICESOFT            reopenDeviceSOFT            = nullptr;
}
namespace al {
    // Core AL function pointers
    LPALDOPPLERFACTOR                dopplerFactor               = nullptr;
    LPALDOPPLERVELOCITY              dopplerVelocity             = nullptr;
    LPALSPEEDOFSOUND                 speedOfSound                = nullptr;
    LPALDISTANCEMODEL                distanceModel               = nullptr;
    LPALENABLE                       enable                      = nullptr;
    LPALDISABLE                      disable                     = nullptr;
    LPALISENABLED                    isEnabled                   = nullptr;
    LPALGETSTRING                    getString                   = nullptr;
    LPALGETBOOLEANV                  getBooleanv                 = nullptr;
    LPALGETINTEGERV                  getIntegerv                 = nullptr;
    LPALGETFLOATV                    getFloatv                   = nullptr;
    LPALGETDOUBLEV                   getDoublev                  = nullptr;
    LPALGETBOOLEAN                   getBoolean                  = nullptr;
    LPALGETINTEGER                   getInteger                  = nullptr;
    LPALGETFLOAT                     getFloat                    = nullptr;
    LPALGETDOUBLE                    getDouble                   = nullptr;
    LPALGETERROR                     getError                    = nullptr;
    LPALISEXTENSIONPRESENT           isExtensionPresent          = nullptr;
    LPALGETPROCADDRESS               getProcAddress              = nullptr;
    LPALGETENUMVALUE                 getEnumValue                = nullptr;
    LPALLISTENERF                    listenerf                   = nullptr;
    LPALLISTENER3F                   listener3f                  = nullptr;
    LPALLISTENERFV                   listenerfv                  = nullptr;
    LPALLISTENERI                    listeneri                   = nullptr;
    LPALLISTENER3I                   listener3i                  = nullptr;
    LPALLISTENERIV                   listeneriv                  = nullptr;
    LPALGETLISTENERF                 getListenerf                = nullptr;
    LPALGETLISTENER3F                getListener3f               = nullptr;
    LPALGETLISTENERFV                getListenerfv               = nullptr;
    LPALGETLISTENERI                 getListeneri                = nullptr;
    LPALGETLISTENER3I                getListener3i               = nullptr;
    LPALGETLISTENERIV                getListeneriv               = nullptr;
    LPALGENSOURCES                   genSources                  = nullptr;
    LPALDELETESOURCES                deleteSources               = nullptr;
    LPALISSOURCE                     isSource                    = nullptr;
    LPALSOURCEF                      sourcef                     = nullptr;
    LPALSOURCE3F                     source3f                    = nullptr;
    LPALSOURCEFV                     sourcefv                    = nullptr;
    LPALSOURCEI                      sourcei                     = nullptr;
    LPALSOURCE3I                     source3i                    = nullptr;
    LPALSOURCEIV                     sourceiv                    = nullptr;
    LPALGETSOURCEF                   getSourcef                  = nullptr;
    LPALGETSOURCE3F                  getSource3f                 = nullptr;
    LPALGETSOURCEFV                  getSourcefv                 = nullptr;
    LPALGETSOURCEI                   getSourcei                  = nullptr;
    LPALGETSOURCE3I                  getSource3i                 = nullptr;
    LPALGETSOURCEIV                  getSourceiv                 = nullptr;
    LPALSOURCEPLAYV                  sourcePlayv                 = nullptr;
    LPALSOURCESTOPV                  sourceStopv                 = nullptr;
    LPALSOURCEREWINDV                sourceRewindv               = nullptr;
    LPALSOURCEPAUSEV                 sourcePausev                = nullptr;
    LPALSOURCEPLAY                   sourcePlay                  = nullptr;
    LPALSOURCESTOP                   sourceStop                  = nullptr;
    LPALSOURCEREWIND                 sourceRewind                = nullptr;
    LPALSOURCEPAUSE                  sourcePause                 = nullptr;
    LPALSOURCEQUEUEBUFFERS           sourceQueueBuffers          = nullptr;
    LPALSOURCEUNQUEUEBUFFERS         sourceUnqueueBuffers        = nullptr;
    LPALGENBUFFERS                   genBuffers                  = nullptr;
    LPALDELETEBUFFERS                deleteBuffers               = nullptr;
    LPALISBUFFER                     isBuffer                    = nullptr;
    LPALBUFFERDATA                   bufferData                  = nullptr;
    LPALBUFFERF                      bufferf                     = nullptr;
    LPALBUFFER3F                     buffer3f                    = nullptr;
    LPALBUFFERFV                     bufferfv                    = nullptr;
    LPALBUFFERI                      bufferi                     = nullptr;
    LPALBUFFER3I                     buffer3i                    = nullptr;
    LPALBUFFERIV                     bufferiv                    = nullptr;
    LPALGETBUFFERF                   getBufferf                  = nullptr;
    LPALGETBUFFER3F                  getBuffer3f                 = nullptr;
    LPALGETBUFFERFV                  getBufferfv                 = nullptr;
    LPALGETBUFFERI                   getBufferi                  = nullptr;
    LPALGETBUFFER3I                  getBuffer3i                 = nullptr;
    LPALGETBUFFERIV                  getBufferiv                 = nullptr;

    // EFX function pointers
    LPALGENEFFECTS                   genEffects                  = nullptr;
    LPALDELETEEFFECTS                deleteEffects               = nullptr;
    LPALISEFFECT                     isEffect                    = nullptr;
    LPALEFFECTI                      effecti                     = nullptr;
    LPALEFFECTIV                     effectiv                    = nullptr;
    LPALEFFECTF                      effectf                     = nullptr;
    LPALEFFECTFV                     effectfv                    = nullptr;
    LPALGETEFFECTI                   getEffecti                  = nullptr;
    LPALGETEFFECTIV                  getEffectiv                 = nullptr;
    LPALGETEFFECTF                   getEffectf                  = nullptr;
    LPALGETEFFECTFV                  getEffectfv                 = nullptr;
    LPALGENFILTERS                   genFilters                  = nullptr;
    LPALDELETEFILTERS                deleteFilters               = nullptr;
    LPALISFILTER                     isFilter                    = nullptr;
    LPALFILTERI                      filteri                     = nullptr;
    LPALFILTERIV                     filteriv                    = nullptr;
    LPALFILTERF                      filterf                     = nullptr;
    LPALFILTERFV                     filterfv                    = nullptr;
    LPALGETFILTERI                   getFilteri                  = nullptr;
    LPALGETFILTERIV                  getFilteriv                 = nullptr;
    LPALGETFILTERF                   getFilterf                  = nullptr;
    LPALGETFILTERFV                  getFilterfv                 = nullptr;
    LPALGENAUXILIARYEFFECTSLOTS      genAuxiliaryEffectSlots     = nullptr;
    LPALDELETEAUXILIARYEFFECTSLOTS   deleteAuxiliaryEffectSlots  = nullptr;
    LPALISAUXILIARYEFFECTSLOT        isAuxiliaryEffectSlot       = nullptr;
    LPALAUXILIARYEFFECTSLOTI         auxiliaryEffectSloti        = nullptr;
    LPALAUXILIARYEFFECTSLOTIV        auxiliaryEffectSlotiv       = nullptr;
    LPALAUXILIARYEFFECTSLOTF         auxiliaryEffectSlotf        = nullptr;
    LPALAUXILIARYEFFECTSLOTFV        auxiliaryEffectSlotfv       = nullptr;
    LPALGETAUXILIARYEFFECTSLOTI      getAuxiliaryEffectSloti     = nullptr;
    LPALGETAUXILIARYEFFECTSLOTIV     getAuxiliaryEffectSlotiv    = nullptr;
    LPALGETAUXILIARYEFFECTSLOTF      getAuxiliaryEffectSlotf     = nullptr;
    LPALGETAUXILIARYEFFECTSLOTFV     getAuxiliaryEffectSlotfv    = nullptr;

    //AL extension function pointers
    //AL_EXT_STATIC_BUFFER
    PFNALBUFFERDATASTATICPROC        bufferDataStatic            = nullptr;
    //AL_SOFT_buffer_sub_data
    PFNALBUFFERSUBDATASOFTPROC       bufferSubDataSOFT           = nullptr;
    //AL_EXT_FOLDBACK
    LPALREQUESTFOLDBACKSTART         requestFoldbackStart        = nullptr;
    LPALREQUESTFOLDBACKSTOP          requestFoldbackStop         = nullptr;
    //AL_SOFT_buffer_samples
    LPALBUFFERSAMPLESSOFT            bufferSamplesSOFT           = nullptr;
    LPALBUFFERSUBSAMPLESSOFT         bufferSubSamplesSOFT        = nullptr;
    LPALGETBUFFERSAMPLESSOFT         getBufferSamplesSOFT        = nullptr;
    LPALISBUFFERFORMATSUPPORTEDSOFT  isBufferFormatSupportedSOFT = nullptr;
    //AL_SOFT_source_latency
    LPALSOURCEDSOFT                  sourcedSOFT                 = nullptr;
    LPALSOURCE3DSOFT                 source3dSOFT                = nullptr;
    LPALSOURCEDVSOFT                 sourcedvSOFT                = nullptr;
    LPALGETSOURCEDSOFT               getSourcedSOFT              = nullptr;
    LPALGETSOURCE3DSOFT              getSource3dSOFT             = nullptr;
    LPALGETSOURCEDVSOFT              getSourcedvSOFT             = nullptr;
    LPALSOURCEI64SOFT                sourcei64SOFT               = nullptr;
    LPALSOURCE3I64SOFT               source3i64SOFT              = nullptr;
    LPALSOURCEI64VSOFT               sourcei64vSOFT              = nullptr;
    LPALGETSOURCEI64SOFT             getSourcei64SOFT            = nullptr;
    LPALGETSOURCE3I64SOFT            getSource3i64SOFT           = nullptr;
    LPALGETSOURCEI64VSOFT            getSourcei64vSOFT           = nullptr;
    //AL_SOFT_deferred_updates
    LPALDEFERUPDATESSOFT             deferUpdatesSOFT            = nullptr;
    LPALPROCESSUPDATESSOFT           processUpdatesSOFT          = nullptr;
    //AL_SOFT_source_resampler
    LPALGETSTRINGISOFT               getStringiSOFT              = nullptr;
    //AL_SOFT_events
    LPALEVENTCONTROLSOFT             eventControlSOFT            = nullptr;
    LPALEVENTCALLBACKSOFT            eventCallbackSOFT           = nullptr;
    LPALGETPOINTERSOFT               getPointerSOFT              = nullptr;
    LPALGETPOINTERVSOFT              getPointervSOFT             = nullptr;
    //AL_SOFT_callback_buffer
    LPALBUFFERCALLBACKSOFT           bufferCallbackSOFT          = nullptr;
    LPALGETBUFFERPTRSOFT             getBufferPtrSOFT            = nullptr;
    LPALGETBUFFER3PTRSOFT            getBuffer3PtrSOFT           = nullptr;
    LPALGETBUFFERPTRVSOFT            getBufferPtrvSOFT           = nullptr;
}

// error handling
namespace alc {
    enum errc {
        no_error = ALC_NO_ERROR,
        invalid_device = ALC_INVALID_DEVICE,
        invalid_context = ALC_INVALID_CONTEXT,
        invalid_enum = ALC_INVALID_ENUM,
        invalid_value = ALC_INVALID_VALUE,
        out_of_memory = ALC_OUT_OF_MEMORY
    };

    AL_HPP_NODISCARD std::string error_message(alc::errc code) AL_HPP_NOEXCEPT {
        switch(code) {
            case alc::errc::no_error: return "No ALC Error found."; break;
            case alc::errc::invalid_device: return "A bad (invalid) device was passed to an ALC function."; break;
            case alc::errc::invalid_context: return "A bad (invalid) context was passed to an ALC function."; break;
            case alc::errc::invalid_enum: return "An unknown (invalid) enum value was passed to an ALC function."; break;
            case alc::errc::invalid_value: return "An invalid value was passed to an ALC function."; break;
            case alc::errc::out_of_memory: return "The requested (ALC) operation resulted in OpenAL running out of memory."; break;
        }
    }

    // not nodiscard, because it can be used to just throw the exception if exceptions are on
    alc::errc check_error(ALCdevice* deviceptr = nullptr, std::string message = "") AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
        alc::errc code = static_cast<alc::errc>(alc::getError(deviceptr));
        if(code != no_error)
            AL_HPP_THROW_WHEN_EXCEPTIONS("ALC Error: " + alc::error_message(code) + message);
        return code;
    }
}
namespace al {
    enum errc {
        no_error = AL_NO_ERROR,
        invalid_name = AL_INVALID_NAME,
        invalid_enum = AL_INVALID_ENUM,
        invalid_value = AL_INVALID_VALUE,
        invalid_operation = AL_INVALID_OPERATION,
        out_of_memory = AL_OUT_OF_MEMORY
    };

    AL_HPP_NODISCARD std::string error_message(al::errc code) AL_HPP_NOEXCEPT {
        switch(code) {
            case al::errc::no_error: return "No AL Error found."; break;
            case al::errc::invalid_name: return "A bad name (ID) was passed to an OpenAL function."; break;
            case al::errc::invalid_enum: return "An invalid enum value was passed to an OpenAL function."; break;
            case al::errc::invalid_value: return "An invalid value was passed to an OpenAL function."; break;
            case al::errc::invalid_operation: return "The requested (AL) operation is not valid."; break;
            case al::errc::out_of_memory: return "The requested (AL) operation resulted in OpenAL running out of memory."; break;
        }
    }

    // not nodiscard, because it can be used to just throw the exception if exceptions are on
    al::errc check_error(std::string message = "") AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
        al::errc code = static_cast<al::errc>(al::getError());
        if(code != no_error)
            AL_HPP_THROW_WHEN_EXCEPTIONS("AL Error: " + al::error_message(code) + message);
        return code;
    }
}

// general use enums and straucts
namespace al {
    enum distance_model {
        none = AL_NONE,
        inverse = AL_INVERSE_DISTANCE,
        inverse_clamped = AL_INVERSE_DISTANCE_CLAMPED,
        linear = AL_LINEAR_DISTANCE,
        linear_clamped = AL_LINEAR_DISTANCE_CLAMPED,
        exponent = AL_EXPONENT_DISTANCE,
        exponent_clamped = AL_EXPONENT_DISTANCE_CLAMPED
    };

    enum format {
        mono8 = AL_FORMAT_MONO8,
        mono16 = AL_FORMAT_MONO16,
        stereo8 = AL_FORMAT_STEREO8,
        stereo16 = AL_FORMAT_STEREO16
    };

    struct vec3 {
        float x, y, z;
    };
}


// function pointer loading
namespace al {
    namespace util {
        class library {
        public:
            library(std::vector<AL_HPP_PATH> library_paths) AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
                for(const auto& path : library_paths) {
                    library_handle = AL_HPP_DYNAMIC_LOADER_LOAD_FUNCTION(path.c_str());
                    // if we fould a valid library, don't open another one
                    if(library_handle != nullptr) return;
                }

                AL_HPP_THROW_WHEN_EXCEPTIONS("No valid library found!");
            }

            // don't copy library handles, we don't want double free
            library(const library&) = delete;
            library& operator=(const library&) = delete;

            // instead. use move operations
            library(library&& old_library) : library_handle{old_library.library_handle} {
                old_library.library_handle = nullptr;
            }
            library& operator=(library&& old_libary) {
                library_handle = old_libary.library_handle;
                old_libary.library_handle = nullptr;
                return *this;
            }
            
            ~library() AL_HPP_NOEXCEPT {
                if(library_handle != nullptr)
                    AL_HPP_DYNAMIC_LOADER_CLOSE_FUNCTION(library_handle);
            }

            template<typename T>
            AL_HPP_INLINE AL_HPP_NODISCARD T sym(const char* name) const AL_HPP_NOEXCEPT {
                return reinterpret_cast<T>(AL_HPP_DYNAMIC_LOADER_SYMBOL_FUNCTION(library_handle, name));
            }

        private:
            AL_HPP_DYNAMIC_LOADER_MODULE_TYPE library_handle = nullptr;
        };

        //convenience function template for function loading:
        template<typename T>
        AL_HPP_INLINE AL_HPP_NODISCARD T al_sym(const char* name) AL_HPP_NOEXCEPT {
            return reinterpret_cast<T>(al::getProcAddress(name));
        }
        template<typename T>
        AL_HPP_INLINE AL_HPP_NODISCARD T alc_sym(const char* name, const ALCdevice* alc_device_ptr) AL_HPP_NOEXCEPT {
            return reinterpret_cast<T>(alc::getProcAddress(alc_device_ptr, name));
        }

        void load_core_alc_from_module() AL_HPP_NOEXCEPT {
            alc::createContext               = al::library->sym<LPALCCREATECONTEXT      >("alcCreateContext");
            alc::makeContextCurrent          = al::library->sym<LPALCMAKECONTEXTCURRENT >("alcMakeContextCurrent");
            alc::processContext              = al::library->sym<LPALCPROCESSCONTEXT     >("alcProcessContext");
            alc::suspendContext              = al::library->sym<LPALCSUSPENDCONTEXT     >("alcSuspendContext");
            alc::destroyContext              = al::library->sym<LPALCDESTROYCONTEXT     >("alcDestroyContext");
            alc::getCurrentContext           = al::library->sym<LPALCGETCURRENTCONTEXT  >("alcGetCurrentContext");
            alc::getContextsDevice           = al::library->sym<LPALCGETCONTEXTSDEVICE  >("alcGetContextsDevice");
            alc::openDevice                  = al::library->sym<LPALCOPENDEVICE         >("alcOpenDevice");
            alc::closeDevice                 = al::library->sym<LPALCCLOSEDEVICE        >("alcCloseDevice");
            alc::getError                    = al::library->sym<LPALCGETERROR           >("alcGetError");
            alc::isExtensionPresent          = al::library->sym<LPALCISEXTENSIONPRESENT >("alcIsExtensionPresent");
            alc::getProcAddress              = al::library->sym<LPALCGETPROCADDRESS     >("alcGetProcAddress");
            alc::getEnumValue                = al::library->sym<LPALCGETENUMVALUE       >("alcGetEnumValue");
            alc::getString                   = al::library->sym<LPALCGETSTRING          >("alcGetString");
            alc::getIntegerv                 = al::library->sym<LPALCGETINTEGERV        >("alcGetIntegerv");
            alc::captureOpenDevice           = al::library->sym<LPALCCAPTUREOPENDEVICE  >("alcCaptureOpenDevice");
            alc::captureCloseDevice          = al::library->sym<LPALCCAPTURECLOSEDEVICE >("alcCaptureCloseDevice");
            alc::captureStart                = al::library->sym<LPALCCAPTURESTART       >("alcCaptureStart");
            alc::captureStop                 = al::library->sym<LPALCCAPTURESTOP        >("alcCaptureStop");
            alc::captureSamples              = al::library->sym<LPALCCAPTURESAMPLES     >("alcCaptureSamples");
        }
        void load_core_al_base_from_module() AL_HPP_NOEXCEPT {
            al::enable                       = al::library->sym<LPALENABLE              >("alEnable");
            al::disable                      = al::library->sym<LPALDISABLE             >("alDisable");
            al::isEnabled                    = al::library->sym<LPALISENABLED           >("alIsEnabled");
            al::getString                    = al::library->sym<LPALGETSTRING           >("alGetString");
            al::getBooleanv                  = al::library->sym<LPALGETBOOLEANV         >("alGetBooleanv");
            al::getIntegerv                  = al::library->sym<LPALGETINTEGERV         >("alGetIntegerv");
            al::getFloatv                    = al::library->sym<LPALGETFLOATV           >("alGetFloatv");
            al::getDoublev                   = al::library->sym<LPALGETDOUBLEV          >("alGetDoublev");
            al::getBoolean                   = al::library->sym<LPALGETBOOLEAN          >("alGetBoolean");
            al::getInteger                   = al::library->sym<LPALGETINTEGER          >("alGetInteger");
            al::getFloat                     = al::library->sym<LPALGETFLOAT            >("alGetFloat");
            al::getDouble                    = al::library->sym<LPALGETDOUBLE           >("alGetDouble");
            al::getError                     = al::library->sym<LPALGETERROR            >("alGetError");
            al::isExtensionPresent           = al::library->sym<LPALISEXTENSIONPRESENT  >("alIsExtensionPresent");
            al::getProcAddress               = al::library->sym<LPALGETPROCADDRESS      >("alGetProcAddress");
            al::getEnumValue                 = al::library->sym<LPALGETENUMVALUE        >("alGetEnumValue");
        }
        void load_core_al_rest_from_module() AL_HPP_NOEXCEPT {
            al::dopplerFactor                = al::library->sym<LPALDOPPLERFACTOR       >("aldopplerFactor");  
            al::dopplerVelocity              = al::library->sym<LPALDOPPLERVELOCITY     >("aldopplerVelocity");  
            al::speedOfSound                 = al::library->sym<LPALSPEEDOFSOUND        >("alspeedOfSound"); 
            al::distanceModel                = al::library->sym<LPALDISTANCEMODEL       >("aldistanceModel");
            al::listenerf                    = al::library->sym<LPALLISTENERF           >("allistenerf");
            al::listener3f                   = al::library->sym<LPALLISTENER3F          >("allistener3f");
            al::listenerfv                   = al::library->sym<LPALLISTENERFV          >("allistenerfv");
            al::listeneri                    = al::library->sym<LPALLISTENERI           >("allisteneri");
            al::listener3i                   = al::library->sym<LPALLISTENER3I          >("allistener3i");
            al::listeneriv                   = al::library->sym<LPALLISTENERIV          >("allisteneriv");
            al::getListenerf                 = al::library->sym<LPALGETLISTENERF        >("algetListenerf");
            al::getListener3f                = al::library->sym<LPALGETLISTENER3F       >("algetListener3f");
            al::getListenerfv                = al::library->sym<LPALGETLISTENERFV       >("algetListenerfv");
            al::getListeneri                 = al::library->sym<LPALGETLISTENERI        >("algetListeneri");
            al::getListener3i                = al::library->sym<LPALGETLISTENER3I       >("algetListener3i");
            al::getListeneriv                = al::library->sym<LPALGETLISTENERIV       >("algetListeneriv");
            al::genSources                   = al::library->sym<LPALGENSOURCES          >("algenSources");
            al::deleteSources                = al::library->sym<LPALDELETESOURCES       >("aldeleteSources");
            al::isSource                     = al::library->sym<LPALISSOURCE            >("alisSource");
            al::sourcef                      = al::library->sym<LPALSOURCEF             >("alsourcef");
            al::source3f                     = al::library->sym<LPALSOURCE3F            >("alsource3f");
            al::sourcefv                     = al::library->sym<LPALSOURCEFV            >("alsourcefv");
            al::sourcei                      = al::library->sym<LPALSOURCEI             >("alsourcei");
            al::source3i                     = al::library->sym<LPALSOURCE3I            >("alsource3i");
            al::sourceiv                     = al::library->sym<LPALSOURCEIV            >("alsourceiv");
            al::getSourcef                   = al::library->sym<LPALGETSOURCEF          >("algetSourcef");
            al::getSource3f                  = al::library->sym<LPALGETSOURCE3F         >("algetSource3f");
            al::getSourcefv                  = al::library->sym<LPALGETSOURCEFV         >("algetSourcefv");
            al::getSourcei                   = al::library->sym<LPALGETSOURCEI          >("algetSourcei");
            al::getSource3i                  = al::library->sym<LPALGETSOURCE3I         >("algetSource3i");
            al::getSourceiv                  = al::library->sym<LPALGETSOURCEIV         >("algetSourceiv");
            al::sourcePlayv                  = al::library->sym<LPALSOURCEPLAYV         >("alsourcePlayv");
            al::sourceStopv                  = al::library->sym<LPALSOURCESTOPV         >("alsourceStopv");
            al::sourceRewindv                = al::library->sym<LPALSOURCEREWINDV       >("alsourceRewindv");
            al::sourcePausev                 = al::library->sym<LPALSOURCEPAUSEV        >("alsourcePausev");
            al::sourcePlay                   = al::library->sym<LPALSOURCEPLAY          >("alsourcePlay");
            al::sourceStop                   = al::library->sym<LPALSOURCESTOP          >("alsourceStop");
            al::sourceRewind                 = al::library->sym<LPALSOURCEREWIND        >("alsourceRewind");
            al::sourcePause                  = al::library->sym<LPALSOURCEPAUSE         >("alsourcePause");
            al::sourceQueueBuffers           = al::library->sym<LPALSOURCEQUEUEBUFFERS  >("alsourceQueueBuffers");
            al::sourceUnqueueBuffers         = al::library->sym<LPALSOURCEUNQUEUEBUFFERS>("alsourceUnqueueBuffers");
            al::genBuffers                   = al::library->sym<LPALGENBUFFERS          >("algenBuffers");
            al::deleteBuffers                = al::library->sym<LPALDELETEBUFFERS       >("aldeleteBuffers");
            al::isBuffer                     = al::library->sym<LPALISBUFFER            >("alisBuffer");
            al::bufferData                   = al::library->sym<LPALBUFFERDATA          >("albufferData");
            al::bufferf                      = al::library->sym<LPALBUFFERF             >("albufferf");
            al::buffer3f                     = al::library->sym<LPALBUFFER3F            >("albuffer3f");
            al::bufferfv                     = al::library->sym<LPALBUFFERFV            >("albufferfv");
            al::bufferi                      = al::library->sym<LPALBUFFERI             >("albufferi");
            al::buffer3i                     = al::library->sym<LPALBUFFER3I            >("albuffer3i");
            al::bufferiv                     = al::library->sym<LPALBUFFERIV            >("albufferiv");
            al::getBufferf                   = al::library->sym<LPALGETBUFFERF          >("algetBufferf");
            al::getBuffer3f                  = al::library->sym<LPALGETBUFFER3F         >("algetBuffer3f");
            al::getBufferfv                  = al::library->sym<LPALGETBUFFERFV         >("algetBufferfv");
            al::getBufferi                   = al::library->sym<LPALGETBUFFERI          >("algetBufferi");
            al::getBuffer3i                  = al::library->sym<LPALGETBUFFER3I         >("algetBuffer3i");
            al::getBufferiv                  = al::library->sym<LPALGETBUFFERIV         >("algetBufferiv");
        }

        void load_alc_extensions_from_deviceptr(const ALCdevice* device_ptr) AL_HPP_NOEXCEPT {
            //ALC_EXT_thread_local_context
            alc::setThreadContext            = alc_sym<PFNALCSETTHREADCONTEXTPROC>("alcSetThreadContext", device_ptr);
            alc::getThreadContext            = alc_sym<PFNALCGETTHREADCONTEXTPROC>("alcGetThreadContext", device_ptr);
            //ALC_SOFT_loopback
            alc::loopbackOpenDeviceSOFT      = alc_sym<LPALCLOOPBACKOPENDEVICESOFT>("alcLoopbackOpenDeviceSOFT", device_ptr);
            alc::isRenderFormatSupportedSOFT = alc_sym<LPALCISRENDERFORMATSUPPORTEDSOFT>("alcIsRenderFormatSupportedSOFT", device_ptr);
            alc::renderSamplesSOFT           = alc_sym<LPALCRENDERSAMPLESSOFT>("alcRenderSamplesSOFT", device_ptr);
            //ALC_SOFT_pause_device
            alc::devicePauseSOFT             = alc_sym<LPALCDEVICEPAUSESOFT>("alcDevicePauseSOFT", device_ptr);
            alc::deviceResumeSOFT            = alc_sym<LPALCDEVICERESUMESOFT>("alcDeviceResumeSOFT", device_ptr);
            //ALC_SOFT_HRTF
            alc::getStringiSOFT              = alc_sym<LPALCGETSTRINGISOFT>("alcGetStringiSOFT", device_ptr);
            alc::resetDeviceSOFT             = alc_sym<LPALCRESETDEVICESOFT>("alcResetDeviceSOFT", device_ptr);
            //ALC_SOFT_device_clock
            alc::getInteger64vSOFT           = alc_sym<LPALCGETINTEGER64VSOFT>("alcGetInteger64vSOFT", device_ptr);
            //ALC_SOFT_reopen_device
            alc::reopenDeviceSOFT            = alc_sym<LPALCREOPENDEVICESOFT>("alcReopenDeviceSOFT", device_ptr);
        }
        void load_efx_from_al() AL_HPP_NOEXCEPT {
            al::genEffects                      = al_sym<LPALGENEFFECTS                >("alGenEffects                ");
            al::deleteEffects                   = al_sym<LPALDELETEEFFECTS             >("alDeleteEffects             ");
            al::isEffect                        = al_sym<LPALISEFFECT                  >("alIsEffect                  ");
            al::effecti                         = al_sym<LPALEFFECTI                   >("alEffecti                   ");
            al::effectiv                        = al_sym<LPALEFFECTIV                  >("alEffectiv                  ");
            al::effectf                         = al_sym<LPALEFFECTF                   >("alEffectf                   ");
            al::effectfv                        = al_sym<LPALEFFECTFV                  >("alEffectfv                  ");
            al::getEffecti                      = al_sym<LPALGETEFFECTI                >("alGetEffecti                ");
            al::getEffectiv                     = al_sym<LPALGETEFFECTIV               >("alGetEffectiv               ");
            al::getEffectf                      = al_sym<LPALGETEFFECTF                >("alGetEffectf                ");
            al::getEffectfv                     = al_sym<LPALGETEFFECTFV               >("alGetEffectfv               ");
            al::genFilters                      = al_sym<LPALGENFILTERS                >("alGenFilters                ");
            al::deleteFilters                   = al_sym<LPALDELETEFILTERS             >("alDeleteFilters             ");
            al::isFilter                        = al_sym<LPALISFILTER                  >("alIsFilter                  ");
            al::filteri                         = al_sym<LPALFILTERI                   >("alFilteri                   ");
            al::filteriv                        = al_sym<LPALFILTERIV                  >("alFilteriv                  ");
            al::filterf                         = al_sym<LPALFILTERF                   >("alFilterf                   ");
            al::filterfv                        = al_sym<LPALFILTERFV                  >("alFilterfv                  ");
            al::getFilteri                      = al_sym<LPALGETFILTERI                >("alGetFilteri                ");
            al::getFilteriv                     = al_sym<LPALGETFILTERIV               >("alGetFilteriv               ");
            al::getFilterf                      = al_sym<LPALGETFILTERF                >("alGetFilterf                ");
            al::getFilterfv                     = al_sym<LPALGETFILTERFV               >("alGetFilterfv               ");
            al::genAuxiliaryEffectSlots         = al_sym<LPALGENAUXILIARYEFFECTSLOTS   >("alGenAuxiliaryEffectSlots   ");
            al::deleteAuxiliaryEffectSlots      = al_sym<LPALDELETEAUXILIARYEFFECTSLOTS>("alDeleteAuxiliaryEffectSlots");
            al::isAuxiliaryEffectSlot           = al_sym<LPALISAUXILIARYEFFECTSLOT     >("alIsAuxiliaryEffectSlot     ");
            al::auxiliaryEffectSloti            = al_sym<LPALAUXILIARYEFFECTSLOTI      >("alAuxiliaryEffectSloti      ");
            al::auxiliaryEffectSlotiv           = al_sym<LPALAUXILIARYEFFECTSLOTIV     >("alAuxiliaryEffectSlotiv     ");
            al::auxiliaryEffectSlotf            = al_sym<LPALAUXILIARYEFFECTSLOTF      >("alAuxiliaryEffectSlotf      ");
            al::auxiliaryEffectSlotfv           = al_sym<LPALAUXILIARYEFFECTSLOTFV     >("alAuxiliaryEffectSlotfv     ");
            al::getAuxiliaryEffectSloti         = al_sym<LPALGETAUXILIARYEFFECTSLOTI   >("alGetAuxiliaryEffectSloti   ");
            al::getAuxiliaryEffectSlotiv        = al_sym<LPALGETAUXILIARYEFFECTSLOTIV  >("alGetAuxiliaryEffectSlotiv  ");
            al::getAuxiliaryEffectSlotf         = al_sym<LPALGETAUXILIARYEFFECTSLOTF   >("alGetAuxiliaryEffectSlotf   ");
            al::getAuxiliaryEffectSlotfv        = al_sym<LPALGETAUXILIARYEFFECTSLOTFV  >("alGetAuxiliaryEffectSlotfv  ");
        }
        void load_al_extensions_from_al() AL_HPP_NOEXCEPT {
            //AL_EXT_STATIC_BUFFER
            al::bufferDataStatic             = al_sym<PFNALBUFFERDATASTATICPROC>("alBufferDataStatic");
            //AL_SOFT_buffer_sub_data
            al::bufferSubDataSOFT            = al_sym<PFNALBUFFERSUBDATASOFTPROC>("alBufferSubDataSOFT");
            //AL_EXT_FOLDBACK
            al::requestFoldbackStart         = al_sym<LPALREQUESTFOLDBACKSTART>("alRequestFoldbackStart");
            al::requestFoldbackStop          = al_sym<LPALREQUESTFOLDBACKSTOP >("alRequestFoldbackStop");
            //AL_SOFT_buffer_samples
            al::bufferSamplesSOFT            = al_sym<LPALBUFFERSAMPLESSOFT          >("alBufferSamplesSOFT");
            al::bufferSubSamplesSOFT         = al_sym<LPALBUFFERSUBSAMPLESSOFT       >("alBufferSubSamplesSOFT");
            al::getBufferSamplesSOFT         = al_sym<LPALGETBUFFERSAMPLESSOFT       >("alGetBufferSamplesSOFT");
            al::isBufferFormatSupportedSOFT  = al_sym<LPALISBUFFERFORMATSUPPORTEDSOFT>("alIsBufferFormatSupportedSOFT");
            //AL_SOFT_source_latency
            al::sourcedSOFT                  = al_sym<LPALSOURCEDSOFT      >("alSourcedSOFT");
            al::source3dSOFT                 = al_sym<LPALSOURCE3DSOFT     >("alSource3dSOFT");
            al::sourcedvSOFT                 = al_sym<LPALSOURCEDVSOFT     >("alSourcedvSOFT");
            al::getSourcedSOFT               = al_sym<LPALGETSOURCEDSOFT   >("alGetSourcedSOFT");
            al::getSource3dSOFT              = al_sym<LPALGETSOURCE3DSOFT  >("alGetSource3dSOFT");
            al::getSourcedvSOFT              = al_sym<LPALGETSOURCEDVSOFT  >("alGetSourcedvSOFT");
            al::sourcei64SOFT                = al_sym<LPALSOURCEI64SOFT    >("alSourcei64SOFT");
            al::source3i64SOFT               = al_sym<LPALSOURCE3I64SOFT   >("alSource3i64SOFT");
            al::sourcei64vSOFT               = al_sym<LPALSOURCEI64VSOFT   >("alSourcei64vSOFT");
            al::getSourcei64SOFT             = al_sym<LPALGETSOURCEI64SOFT >("alGetSourcei64SOFT");
            al::getSource3i64SOFT            = al_sym<LPALGETSOURCE3I64SOFT>("alGetSource3i64SOFT");
            al::getSourcei64vSOFT            = al_sym<LPALGETSOURCEI64VSOFT>("alGetSourcei64vSOFT");
            //AL_SOFT_deferred_updates
            al::deferUpdatesSOFT             = al_sym<LPALDEFERUPDATESSOFT  >("alDeferUpdatesSOFT");
            al::processUpdatesSOFT           = al_sym<LPALPROCESSUPDATESSOFT>("alProcessUpdatesSOFT");
            //AL_SOFT_source_resampler
            al::getStringiSOFT               = al_sym<LPALGETSTRINGISOFT>("alGetStringiSOFT");
            //AL_SOFT_events
            al::eventControlSOFT             = al_sym<LPALEVENTCONTROLSOFT >("alEventControlSOFT");
            al::eventCallbackSOFT            = al_sym<LPALEVENTCALLBACKSOFT>("alEventCallbackSOFT");
            al::getPointerSOFT               = al_sym<LPALGETPOINTERSOFT   >("alGetPointerSOFT");
            al::getPointervSOFT              = al_sym<LPALGETPOINTERVSOFT  >("alGetPointervSOFT");
            //AL_SOFT_callback_buffer
            al::bufferCallbackSOFT           = al_sym<LPALBUFFERCALLBACKSOFT>("albufferCallbackSOFT");
            al::getBufferPtrSOFT             = al_sym<LPALGETBUFFERPTRSOFT  >("algetBufferPtrSOFT");
            al::getBuffer3PtrSOFT            = al_sym<LPALGETBUFFER3PTRSOFT >("algetBuffer3PtrSOFT");
            al::getBufferPtrvSOFT            = al_sym<LPALGETBUFFERPTRVSOFT >("algetBufferPtrvSOFT");
        }

    }

    // this is a singleton with the same name as the class...
    std::unique_ptr<al::util::library> library = nullptr;
    bool loaded = false;

    void load(AL_HPP_PATH library_path = "") AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
        if(al::loaded) return;

        // only load library once
        if(library == nullptr) {
            std::vector<AL_HPP_PATH> paths;
            if(library_path != "") paths.push_back(library_path);
            paths.push_back(AL_HPP_LIBRARY_NAME);
            paths.push_back(AL_HPP_SECONDARY_LIBRARY_NAME);
            library = std::make_unique<al::util::library>(paths);
            
            // for AL_HPP_NO_EXCEPTIONS
            if(library == nullptr) return;
        }

        al::util::load_core_alc_from_module();
        al::util::load_core_al_base_from_module();
        al::util::load_core_al_rest_from_module();

        al::loaded = true;
    }

    void update(const ALCdevice* device_ptr) AL_HPP_NOEXCEPT {
        al::util::load_alc_extensions_from_deviceptr(device_ptr);
        al::util::load_efx_from_al();
        al::util::load_al_extensions_from_al();
    }

    void update() AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
        ALCcontext* current_context = alc::getCurrentContext();
        alc::check_error();
        ALCdevice* current_contexts_device_ptr = alc::getContextsDevice(current_context);
        // does this do the same error check, or is the default error check different from resetting device errors?
        alc::check_error();
        alc::check_error(current_contexts_device_ptr);
        update(current_contexts_device_ptr);
    }

    void unload() AL_HPP_NOEXCEPT {
        library.reset();
    }
}


// object model for OpenAL - devices, buffers, listeners and sources
namespace al {

    namespace util {
        // OpenAL has the aweful convention of having a string array as a single strings with NUL terminators in the middle.
        // see https://www.reddit.com/r/cpp_questions/comments/11qkcx6/is_there_a_stdstringsplit_or_something_similar/
        // built on recommendation of u/PinkOwls_ 
        AL_HPP_NODISCARD std::vector<std::string> split_string_array_copy(const char* str) AL_HPP_NOEXCEPT {
            std::vector<std::string> string_list;
            const char* start = str;
            const char* end = str;
            for(;;) {
                while(*end++);
                if(end == start+1) break;

                string_list.emplace_back(start);
                start = end;
            }
            return string_list;
        }

        AL_HPP_NODISCARD std::vector<nonstd::string_view> split_string_array(const char* str) AL_HPP_NOEXCEPT {
            std::vector<nonstd::string_view> string_list;
            const char* start = str;
            const char* end = str;
            for(;;) {
                while(*end++);
                if(end == start+1) break;

                string_list.emplace_back(start,end-start);
                start = end;
            }
            return string_list;
        }

        // for more sane lists, like the ' '-delimited extension string list
        // built on recommendations by u/Mason-B and u/topological_rabbit respectively in the same thread as above
        AL_HPP_NODISCARD std::vector<std::string> split_string_copy(const std::string& str, const char delim = ' ') AL_HPP_NOEXCEPT {
            std::stringstream input_stream { str };
            std::vector<std::string> string_list;
            std::string current_string;

            while( std::getline( input_stream, current_string, delim ) )
                    string_list.emplace_back( std::move( current_string ) );

            return string_list;
        }

        AL_HPP_NODISCARD std::vector<nonstd::string_view> split_string(const std::string& str, const char delim = ' ') AL_HPP_NOEXCEPT {
            std::vector<nonstd::string_view> string_list;

            size_t begin = 0, end;
            while((end = str.find(delim,begin)) != std::string::npos) {
                string_list.emplace_back(&(str.c_str())[begin], end-begin);
                begin = end;
            }
            if(begin != str.size())
                string_list.emplace_back(&(str.c_str())[begin], str.size()-begin);

            return string_list;
        }

    }

    AL_HPP_NODISCARD std::vector<std::string> device_names() AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
        if(! alc::isExtensionPresent(nullptr, "ALC_ENUMERATION_EXT")) {
            AL_HPP_THROW_WHEN_EXCEPTIONS("Error! Can't enumerate Devices because of missing Core Extension to OpenAL (Enumeration Extension)");
        }
        alc::check_error();

        auto device_names = alc::getString(nullptr, ALC_DEVICE_SPECIFIER);
        alc::check_error();
        return al::util::split_string_array_copy(device_names);
    }

    

    class device {
    public:
        AL_HPP_NODISCARD device(const char* device_name = nullptr) AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
            device_ptr_ = alc::openDevice(device_name);
            alc::check_error(nullptr,"(opening ALC device)");
            alc::check_error(device_ptr_,"(opening ALC device)");
            if(device_ptr_ == nullptr) {
                AL_HPP_THROW_WHEN_EXCEPTIONS("Error: could not open ALC device!");
                return;
            }

            if(device_name != nullptr)
                name_ = std::string(device_name);

            update_info();
        }

        void update_info() AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
            alc::getIntegerv(device_ptr_, ALC_MINOR_VERSION, 1, &alc_version_.minor);
            check();
            alc::getIntegerv(device_ptr_, ALC_MAJOR_VERSION, 1, &alc_version_.major);
            check();

            vendor_ = al::getString(AL_VENDOR);
            al::check_error();
            al_version_ = al::getString(AL_VERSION);
            al::check_error();
            renderer_ = al::getString(AL_RENDERER);
            al::check_error();
            std::string extensions_string = al::getString(AL_EXTENSIONS);
            al::check_error();
            extensions_ = al::util::split_string_copy(extensions_string, ' ');
        }

        // don't copy handlers
        device(const device&) = delete;
        device& operator=(const device&) = delete;

        // instead. use move operations
        device(device&& old_device) : device_ptr_{old_device.device_ptr_},
                                      alc_version_(old_device.alc_version_),
                                      name_(old_device.name_),
                                      vendor_(old_device.vendor_),
                                      al_version_(old_device.al_version_),
                                      renderer_(old_device.renderer_),
                                      extensions_(old_device.extensions_) {
            old_device.device_ptr_ = nullptr;
        }
        device& operator=(device&& old_device) {
            device_ptr_ = old_device.device_ptr_;
            alc_version_ = old_device.alc_version_;
            name_ = old_device.name_;
            vendor_ = old_device.vendor_;
            al_version_ = old_device.al_version_;
            renderer_ = old_device.renderer_;
            extensions_ = old_device.extensions_;

            old_device.device_ptr_ = nullptr;
            return *this;
        }
        


        
        ~device() AL_HPP_NOEXCEPT {
            if(device_ptr_ != nullptr)
                alc::closeDevice(device_ptr_);
        }


        // only checks ALC errors
        void check(std::string message = "") const AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
            alc::check_error(device_ptr_, message);
        }

        // returns non-owning pointer
        AL_HPP_NODISCARD ALCdevice*  device_ptr()   const AL_HPP_NOEXCEPT { return device_ptr_; }

        
        // device information
        AL_HPP_NODISCARD auto        alc_version()  const AL_HPP_NOEXCEPT { return alc_version_; }
        AL_HPP_NODISCARD std::string name()         const AL_HPP_NOEXCEPT { return name_; }
        AL_HPP_NODISCARD std::string vendor()       const AL_HPP_NOEXCEPT { return vendor_; }
        AL_HPP_NODISCARD std::string al_version()   const AL_HPP_NOEXCEPT { return al_version_; }
        AL_HPP_NODISCARD std::string renderer()     const AL_HPP_NOEXCEPT { return renderer_; }
        AL_HPP_NODISCARD auto        extensions()   const AL_HPP_NOEXCEPT { return extensions_; }


    private:
        // owner because the device object owns a _handle_, not an allocation
        gsl::owner<ALCdevice*> device_ptr_ = nullptr;

        struct {
            int major;
            int minor;
        } alc_version_;
        std::string name_ = "", vendor_, al_version_, renderer_;
        std::vector<std::string> extensions_;

    };


    class buffer {
    public:
    
        AL_HPP_NODISCARD buffer(device buffer_device) AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {

        }

    private:

        ALuint buffer_id;
        al::format format;
        ALuint bits, channels;
        const void* data;
        ALsizei size;
        float frequency;
    };

    // The nomenclature is really confusing here, "Listener" is also called "Context", as the context depends on the device, and there is only one listener per context
    // We manage the context in the listener since creating a new listener also automatically involves creating a new context
    // Also, the distance model is clearly part of the Listener, therefore its stored here.
    // However, since information like "Vendor" really describes information about the Device, although its stored in the "context" by spec, we'll be storing it
    // in the device object itself and update it via the update_information method in the listener constructor
    class listener {
    public:
    
        AL_HPP_NODISCARD listener(al::device listener_device) AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
            // TODO: handle attributes (second argument)
            context_ptr_ = alc::createContext(listener_device.device_ptr(), nullptr);
            listener_device.check("(creating ALC context)");
            if(context_ptr_ == nullptr) {
                AL_HPP_THROW_WHEN_EXCEPTIONS("Error: could not create ALC context!");
                return;
            }
            ALCboolean code = alc::makeContextCurrent(context_ptr_);
            listener_device.check("(making ALC context current)");
            if(!code) {
                AL_HPP_THROW_WHEN_EXCEPTIONS("Error: could not make ALC context current!");
                return;
            }

        }

    private:
        ALCcontext* context_ptr_;

        int frequency, refresh;
        bool sync;
        int mono_sources, stereo_sources;
        float doppler_factor, doppler_velocity, speed_of_sound;
        al::distance_model model;

        float gain;
        al::vec3 position, velocity, forward_direction, up_direction;
    };

    class source {
    public:
    
        AL_HPP_NODISCARD source(listener listener_source) AL_HPP_NOEXCEPT_WHEN_NO_EXCEPTIONS {
            
        }

        enum type {
            undetermined = AL_UNDETERMINED,
            //clunky name because AL uses a keyword ("static") as an identifier...
            static_source = AL_STATIC,
            streaming_source = AL_STREAMING
        };

        enum state {
            initial = AL_INITIAL,
            playing = AL_PLAYING,
            paused = AL_PAUSED,
            stopped = AL_STOPPED
        };

    private:
        ALuint source_id;

        float pitch, gain;
        float maxDistance, rolloffFactor, referenceDistance;
        float minGain, maxGain;
        float coneOuterGain, coneInnerAngle, coneOuterAngle;
        al::vec3 position, velocity, direction;
        ALboolean sourceRelative;
        type current_type;
        ALboolean looping;
        std::vector<al::buffer> buffers; //a single buffer or buffersQueued+buffersProcessed buffers
        ALuint currentBufferId; //= AL_BUFFER attribute
        state current_state;
        ALint buffersQueued, buffersProcessed;
        float secOffset, sampleOffset, byteOffset;
    };


} // namespace al

#endif
