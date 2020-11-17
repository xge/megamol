#include "stdafx.h"
#include "ParticleColors.h"

#include "mmcore/view/CallGetTransferFunction.h"


megamol::stdplugin::datatools::ParticleColors::ParticleColors(void)
        : AbstractParticleManipulator("outData", "indata"), _tf_slot("inTF", "") {
    _tf_slot.SetCompatibleCall<core::view::CallGetTransferFunctionDescription>();
    MakeSlotAvailable(&_tf_slot);
}


megamol::stdplugin::datatools::ParticleColors::~ParticleColors(void) {
    this->Release();
}


float lerp(float a, float b, float inter) {
    return a * (1.0f - inter) + b * inter;
}


glm::vec4 sample_tf(float const* tf, unsigned int tf_size, int base, float rest) {
    if (base < 0 || tf_size == 0)
        return glm::vec4(0);
    auto const last_el = tf_size - 1;
    if (base >= last_el)
        return glm::vec4(tf[last_el * 4], tf[last_el * 4 + 1], tf[last_el * 4 + 2], tf[last_el * 4 + 3]);

    auto const a = base;
    auto const b = base + 1;

    return glm::vec4(lerp(tf[a * 4], tf[b * 4], rest), lerp(tf[a * 4 + 1], tf[b * 4 + 1], rest),
        lerp(tf[a * 4 + 2], tf[b * 4 + 2], rest), lerp(tf[a * 4 + 3], tf[b * 4 + 3], rest));
}


bool megamol::stdplugin::datatools::ParticleColors::manipulateData(
    core::moldyn::MultiParticleDataCall& outData, core::moldyn::MultiParticleDataCall& inData) {
    outData = inData; // also transfers the unlocker to 'outData'

    inData.SetUnlocker(nullptr, false); // keep original data locked
                                        // original data will be unlocked through outData

    core::view::CallGetTransferFunction* cgtf = _tf_slot.CallAs<core::view::CallGetTransferFunction>();
    if (cgtf == nullptr)
        return false;
    if (!(*cgtf)())
        return false;

    if (_frame_id != inData.FrameID() || _in_data_hash != inData.DataHash() || cgtf->IsDirty()) {
        auto const tf = cgtf->GetTextureData();
        auto const tf_size = cgtf->TextureSize();

        auto const pl_count = outData.GetParticleListCount();
        _colors.clear();
        _colors.resize(pl_count);

        for (unsigned int plidx = 0; plidx < pl_count; ++plidx) {
            auto& parts = outData.AccessParticles(plidx);
            if (parts.GetColourDataType() != core::moldyn::SimpleSphericalParticles::COLDATA_FLOAT_I &&
                parts.GetColourDataType() != core::moldyn::SimpleSphericalParticles::COLDATA_DOUBLE_I)
                continue;

            auto const p_count = parts.GetCount();
            auto& col_vec = _colors[plidx];
            col_vec.resize(p_count);

            auto const min_i = parts.GetMinColourIndexValue();
            auto const max_i = parts.GetMaxColourIndexValue();
            auto const fac_i = 1.0f / (max_i - min_i + 1e-8f);

            auto const iAcc = parts.GetParticleStore().GetCRAcc();

            for (std::size_t pidx = 0; pidx < p_count; ++pidx) {
                auto const val = (iAcc->Get_f(pidx) - min_i) * fac_i;
                std::remove_const_t<decltype(val)> main = 0;
                auto rest = std::modf(val, &main);
                col_vec[pidx].rgba = sample_tf(tf, tf_size, static_cast<int>(tf_size * main), rest);
            }

            parts.SetColourData(core::moldyn::SimpleSphericalParticles::COLDATA_FLOAT_RGBA, col_vec.data());
        }


        _frame_id = inData.FrameID();
        _in_data_hash = inData.DataHash();
        ++_out_data_hash;
        cgtf->ResetDirty();
    }

    outData.SetDataHash(_out_data_hash);

    return true;
}
