Texture2D<float4>   TextureF  : register(t0);
Texture2D<int4>     TextureI  : register(t0);
Texture2D<uint4>    TextureUI : register(t0);

cbuffer PackParams : register(b0)
{
    uint ReadStride;
    uint WriteStride;
    uint AlignmentOffset;
    uint WriteOffset;
    float2 WriteTexOffset;
    float2 WriteTexScale;
}

struct VS_OUTPUT
{
    int3   loadIndex : LOADINDEX;
    float4 position  : SV_Position;
};

void VS_PackPixels(in uint vertexID : SV_VertexID, out VS_OUTPUT outVertex)
{
    uint readY = vertexID / ReadStride;
    uint readX = vertexID - (readY * ReadStride);

    // Pack pixels by skipping the number of rows times alignment skip per row
    uint writeAlignmentOffset = readY * AlignmentOffset;

    // Pixel index we are writing to in the output texture
    uint writeIndex = vertexID + WriteOffset + writeAlignmentOffset;

    uint writeY = writeIndex / WriteStride;
    uint writeX = writeIndex - (writeY * WriteStride);

    float2 writeCoords = float2(float(writeX), float(writeY)) * WriteTexScale + WriteTexOffset;

    outVertex.position  = float4(writeCoords, 0.0f, 1.0f);
    outVertex.loadIndex = int3(readX, readY, 0);
}

float4 PS_PackPixels_4F(in int3 loadIndex : LOADINDEX) : SV_Target
{
    return TextureF.Load(loadIndex);
}

int4 PS_PackPixels_4I(in int3 loadIndex : LOADINDEX) : SV_Target
{
    return TextureI.Load(loadIndex);
}

uint4 PS_PackPixels_4UI(in int3 loadIndex : LOADINDEX) : SV_Target
{
    return TextureUI.Load(loadIndex);
}
