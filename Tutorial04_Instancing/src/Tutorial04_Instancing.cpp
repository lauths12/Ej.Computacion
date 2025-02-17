/*
 *  Copyright 2019-2024 Diligent Graphics LLC
 *  Copyright 2015-2019 Egor Yusov
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include <random>
#include "Tutorial04_Instancing.hpp"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"
#include "ColorConversion.h"
#include "../../Common/src/TexturedCube.hpp"
#include "imgui.h"


namespace Diligent
{

SampleBase* CreateSample()
{
    return new Tutorial04_Instancing();
}

void Tutorial04_Instancing::CreatePipelineState()
{
    LayoutElement LayoutElems[] =
        {
            LayoutElement{0, 0, 3, VT_FLOAT32, False},
            LayoutElement{1, 0, 2, VT_FLOAT32, False},
            LayoutElement{2, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
            LayoutElement{3, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
            LayoutElement{4, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
            LayoutElement{5, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE}};

    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);

    TexturedCube::CreatePSOInfo CubePsoCI;
    CubePsoCI.pDevice              = m_pDevice;
    CubePsoCI.RTVFormat            = m_pSwapChain->GetDesc().ColorBufferFormat;
    CubePsoCI.DSVFormat            = m_pSwapChain->GetDesc().DepthBufferFormat;
    CubePsoCI.pShaderSourceFactory = pShaderSourceFactory;

#ifdef DILIGENT_USE_OPENGL

    CubePsoCI.VSFilePath = "cube_inst_glsl.vert";
    CubePsoCI.PSFilePath = "cube_inst_glsl.frag";
#else
    CubePsoCI.VSFilePath = "cube_inst.vsh";
    CubePsoCI.PSFilePath = "cube_inst.psh";
#endif

    CubePsoCI.ExtraLayoutElements    = LayoutElems;
    CubePsoCI.NumExtraLayoutElements = _countof(LayoutElems);

    m_pPSO = TexturedCube::CreatePipelineState(CubePsoCI, m_ConvertPSOutputToGamma);

    CreateUniformBuffer(m_pDevice, sizeof(float4x4) * 2, "VS constants CB", &m_VSConstants);
    m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);
    m_pPSO->CreateShaderResourceBinding(&m_SRB, true);
}

void Tutorial04_Instancing::CreateInstanceBuffer()
{
    BufferDesc InstBuffDesc;
    InstBuffDesc.Name      = "Instance data buffer";
    InstBuffDesc.Usage     = USAGE_DEFAULT;
    InstBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    InstBuffDesc.Size      = sizeof(float4x4) * 22;
    m_pDevice->CreateBuffer(InstBuffDesc, nullptr, &m_InstanceBuffer);
    PopulateInstanceBuffer();
}

void Tutorial04_Instancing::Initialize(const SampleInitInfo& InitInfo)
{
    SampleBase::Initialize(InitInfo);
    CreatePipelineState();

    m_CubeVertexBuffer = TexturedCube::CreateVertexBuffer(m_pDevice, GEOMETRY_PRIMITIVE_VERTEX_FLAG_POS_TEX);
    m_CubeIndexBuffer  = TexturedCube::CreateIndexBuffer(m_pDevice);
    m_TextureSRV       = TexturedCube::LoadTexture(m_pDevice, "DGLogo.png")->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(m_TextureSRV);

    CreateInstanceBuffer();
}

static float angle = (PI_F / 1.0);

void Tutorial04_Instancing::PopulateInstanceBuffer()
{
    std::vector<float4x4> InstanceData(22);

    const float4x4 scaleHiloVertical = float4x4::Scale(0.01f, 1.0f, 0.01f);

    angle += 0.003;


    InstanceData[0]  = float4x4::Scale(5.0f, 0.1f, 0.01f) * float4x4::Translation(0.0f, 0.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[1]  = float4x4::Scale(0.01f, 0.1f, 5.0f) * float4x4::Translation(0.0f, 0.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[2]  = float4x4::Scale(0.1f, 1.0f, 0.01f) * float4x4::Translation(-5.0f, -1.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[3]  = float4x4::Scale(0.1f, 1.0f, 0.01f) * float4x4::Translation(5.0f, -1.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[4]  = float4x4::Scale(0.1f, 1.0f, 0.01f) * float4x4::Translation(0.0f, 1.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[5]  = float4x4::Scale(0.05f, 1.0f, 0.01f) * float4x4::Translation(0.0f, -1.0f, -5.0f) * float4x4::RotationY(angle);
    InstanceData[6]  = float4x4::Scale(0.05f, 1.0f, 0.01f) * float4x4::Translation(0.0f, -1.0f, 5.0f) * float4x4::RotationY(angle);
    InstanceData[7]  = float4x4::Scale(1, 1, 1) * float4x4::Translation(-5.0f, -2.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[8]  = float4x4::Scale(1, 1, 1) * float4x4::Translation(5.0f, -2.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[9]  = float4x4::Scale(1, 1, 1) * float4x4::Translation(0.0f, -2.0f, -5.0f) * float4x4::RotationY(angle);
    InstanceData[10] = float4x4::Scale(1, 1, 1) * float4x4::Translation(0.0f, -2.0f, 5.0f) * float4x4::RotationY(angle);
    InstanceData[11] = float4x4::Scale(3.0f, 0.05f, 0.01f) * float4x4::Translation(0.0f, -5.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[12] = float4x4::Scale(0.01f, 0.05f, 3.0f) * float4x4::Translation(0.0f, -5.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[13] = float4x4::Scale(0.05f, 4.0f, 0.01f) * float4x4::Translation(0.0f, -1.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[14] = float4x4::Scale(0.05f, 1.0f, 0.01f) * float4x4::Translation(-3.0f, -6.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[15] = float4x4::Scale(0.05f, 1.0f, 0.01f) * float4x4::Translation(3.0f, -6.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[16] = float4x4::Scale(0.05f, 1.0f, 0.01f) * float4x4::Translation(0.0f, -6.0f, 3.0f) * float4x4::RotationY(angle);
    InstanceData[17] = float4x4::Scale(0.05f, 1.0f, 0.01f) * float4x4::Translation(0.0f, -6.0f, -3.0f) * float4x4::RotationY(angle);
    InstanceData[18] = float4x4::Scale(1, 1, 1) * float4x4::Translation(-3.0f, -7.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[19] = float4x4::Scale(1, 1, 1) * float4x4::Translation(3.0f, -7.0f, 0.0f) * float4x4::RotationY(angle);
    InstanceData[20] = float4x4::Scale(1, 1, 1) * float4x4::Translation(0.0f, -7.0f, 3.0f) * float4x4::RotationY(angle);
    InstanceData[21] = float4x4::Scale(1, 1, 1) * float4x4::Translation(0.0f, -7.0f, -3.0f) * float4x4::RotationY(angle);



    Uint32 DataSize = static_cast<Uint32>(sizeof(InstanceData[0]) * InstanceData.size());
    m_pImmediateContext->UpdateBuffer(m_InstanceBuffer, 0, DataSize, InstanceData.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void Tutorial04_Instancing::Render()
{
    auto*  pRTV       = m_pSwapChain->GetCurrentBackBufferRTV();
    auto*  pDSV       = m_pSwapChain->GetDepthBufferDSV();
    float4 ClearColor = {0.0f, 0.0f, 0.0f, 1.0f};


    PopulateInstanceBuffer();

    if (m_ConvertPSOutputToGamma)
        ClearColor = LinearToSRGB(ClearColor);

    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor.Data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    {
        MapHelper<float4x4> CBConstants(m_pImmediateContext, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
        CBConstants[0] = m_ViewProjMatrix;
        CBConstants[1] = m_RotationMatrix;
    }

    const Uint64 offsets[] = {0, 0};
    IBuffer*     pBuffs[]  = {m_CubeVertexBuffer, m_InstanceBuffer};
    m_pImmediateContext->SetVertexBuffers(0, _countof(pBuffs), pBuffs, offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
    m_pImmediateContext->SetIndexBuffer(m_CubeIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    m_pImmediateContext->SetPipelineState(m_pPSO);
    m_pImmediateContext->CommitShaderResources(m_SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    DrawIndexedAttribs DrawAttrs;
    DrawAttrs.IndexType    = VT_UINT32;
    DrawAttrs.NumIndices   = 36;
    DrawAttrs.NumInstances = 22;
    DrawAttrs.Flags        = DRAW_FLAG_VERIFY_ALL;
    m_pImmediateContext->DrawIndexed(DrawAttrs);
}
void Tutorial04_Instancing::Update(double CurrTime, double ElapsedTime)
{
    SampleBase::Update(CurrTime, ElapsedTime);

    static int viewOption = 0;
    ImGui::Begin("Vista de Camara");
    ImGui::Combo("Vista", &viewOption, "Frontal\0Trasera\0Izquierda\0Derecha\0Superior\0Inferior\0Cercana\0Lejana\0");
    ImGui::End();

    float4x4 View;

    if (viewOption == 0)
    { // Frontal
        View = float4x4::Translation(0.f, 3.0f, 20.0f);
    }
    else if (viewOption == 1)
    { // Trasera
        View = float4x4::Translation(0.f, 3.0f, -20.0f) * float4x4::RotationY(PI_F);
    }
    else if (viewOption == 2)
    { // Izquierda
        View = float4x4::Translation(20.f, 3.0f, 0.f) * float4x4::RotationY(-PI_F / 2);
    }
    else if (viewOption == 3)
    { // Derecha
        View = float4x4::Translation(-20.f, 3.0f, 0.f) * float4x4::RotationY(PI_F / 2);
    }
    else if (viewOption == 4)
    { // Superior
        View = float4x4::Translation(0.f, 20.f, 0.f) * float4x4::RotationX(PI_F / 2);
    }
    else if (viewOption == 5)
    { // Inferior
        View = float4x4::Translation(0.f, -20.f, 0.f) * float4x4::RotationX(-PI_F / 2);
    }
    else if (viewOption == 6)
    { // Cercana
        View = float4x4::Translation(0.f, 2.f, 10.f);
    }
    else if (viewOption == 7)
    { // Lejana
        View = float4x4::Translation(0.f, 20.f, 50.f) * float4x4::RotationX(PI_F / 12);
    }
    else
    { // Vista por defecto
        View = float4x4::Translation(0.f, 3.0f, 20.0f);
    }

    auto SrfPreTransform = GetSurfacePretransformMatrix(float3{0, 0, 1});
    auto Proj            = GetAdjustedProjectionMatrix(PI_F / 4.0f, 0.1f, 100.f);
    m_ViewProjMatrix     = View * SrfPreTransform * Proj;
    m_RotationMatrix     = float4x4::Identity();
}



} // namespace Diligent