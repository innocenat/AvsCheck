
#pragma once


#ifndef __AVSENV_H
#define __AVSENV_H

#include <avisynth.h>
#include <windows.h>
#include <memory>

// Predefined class
class AvsEnv;
class AvsScriptEnvironment;
class AvsClip;
class AvsVideoFrame;

// Main avs_linkage that will be swapped for each DLL loaded
extern const AVS_Linkage *AVS_linkage;
extern std::weak_ptr<AvsEnv> __avs_env_global;

#define AVS_MAIN_FILE                                \
    const AVS_Linkage *AVS_linkage = 0;              \
    std::weak_ptr<AvsEnv> __avs_env_global;

#define ENTER_AVISYNTH(avs_env) if (avs_env) avs_env->EnterAvisynth()

class AvsEnv
{
    friend class AvsScriptEnvironment;
public:
    virtual ~AvsEnv() {
        ExitAvisynth();
        env->DeleteScriptEnvironment();
        FreeLibrary(dll);
    };

    static std::shared_ptr<AvsEnv> Create(const char* path) {
        HINSTANCE dll = LoadLibrary(path);

        if (!dll) {
            return std::shared_ptr<AvsEnv>();
        }

        IScriptEnvironment *(__stdcall *CreateEnvironment)(int) =
            (IScriptEnvironment *(__stdcall *)(int)) GetProcAddress(dll, "CreateScriptEnvironment");

        if (!CreateEnvironment) {
            FreeLibrary(dll);
            return std::shared_ptr<AvsEnv>();
        }

        IScriptEnvironment* env = CreateEnvironment(AVISYNTH_INTERFACE_VERSION);

        if (!env) {
            FreeLibrary(dll);
            return std::shared_ptr<AvsEnv>();
        }

        const AVS_Linkage* linkage = env->GetAVSLinkage();

        AvsEnv* p_avs_env = new AvsEnv(linkage, env, dll);
        std::shared_ptr<AvsEnv> avs_env(p_avs_env);
        avs_env->self = avs_env;

        return avs_env;
    }

    void EnterAvisynth() {
        AVS_linkage = linkage;
        __avs_env_global = self;
    }

    static void ExitAvisynth() {
        AVS_linkage = nullptr;
    }

    void DestroyAvisynth() {
        self.reset();
    }

private:
    AvsEnv(const AVS_Linkage* l, IScriptEnvironment* e, HINSTANCE dll) : linkage(l), env(e), dll(dll) {}

    const AVS_Linkage * linkage;
    IScriptEnvironment* env;
    HINSTANCE dll;

    std::weak_ptr<AvsEnv> self;
};

class AvsScriptEnvironment
{
public:
    AvsScriptEnvironment(const AvsScriptEnvironment& env) : env(env.env), avs_env(env.avs_env) {}
    AvsScriptEnvironment(const std::shared_ptr<AvsEnv>& avs_env) : env(avs_env->env), avs_env(avs_env) {}

    virtual ~AvsScriptEnvironment() {}

    void operator=(const AvsScriptEnvironment& a) {
        env = a.env;
        avs_env = a.avs_env;
    }

    IScriptEnvironment* operator->() {
        ENTER_AVISYNTH(avs_env);
        return env;
    }

    operator IScriptEnvironment*() {
        ENTER_AVISYNTH(avs_env);
        return env;
    }

private:
    AvsScriptEnvironment(IScriptEnvironment* env) : env(env), avs_env(__avs_env_global.lock()) {}
    IScriptEnvironment* env;
    std::shared_ptr<AvsEnv> avs_env;
};

class AvsClip
{
public:
    AvsClip() : clip(), avs_env() {}
    AvsClip(PClip clip) : clip(clip), avs_env(__avs_env_global.lock()) {}
    AvsClip(const AvsClip& clip) : clip(clip.clip), avs_env(clip.avs_env) {}

    virtual ~AvsClip() {
        ENTER_AVISYNTH(avs_env);
    }

    // TODO this probably need fix
    void operator=(const AvsClip& c) {
        ENTER_AVISYNTH(c.avs_env);

        clip = c.clip;
        avs_env = c.avs_env;
    }

    PClip operator->() const {
        ENTER_AVISYNTH(avs_env);
        return clip;
    }

    operator PClip() {
        ENTER_AVISYNTH(avs_env);
        return clip;
    }

private:
    PClip clip;
    std::shared_ptr<AvsEnv> avs_env;
};

class AvsVideoFrame
{
public:
    AvsVideoFrame(PVideoFrame frame) : frame(frame), avs_env(__avs_env_global.lock()) {}
    AvsVideoFrame(const AvsVideoFrame& frame) : frame(frame.frame), avs_env(frame.avs_env) {}

    virtual ~AvsVideoFrame() {
        ENTER_AVISYNTH(avs_env);
    }

    // TODO this probably need fix
    void operator=(const AvsVideoFrame& c) {
        ENTER_AVISYNTH(c.avs_env);

        frame = c.frame;
        avs_env = c.avs_env;
    }

    PVideoFrame operator->() const {
        ENTER_AVISYNTH(avs_env);
        return frame;
    }

    operator PVideoFrame() {
        ENTER_AVISYNTH(avs_env);
        return frame;
    }

private:
    PVideoFrame frame;
    std::shared_ptr<AvsEnv> avs_env;
};

#undef ENTER_AVISYNTH

#endif
