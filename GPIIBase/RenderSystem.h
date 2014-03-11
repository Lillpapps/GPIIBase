// RenderSystem.h

#pragma once

#define INVALID_RESOURCE -1

enum EBufferAccess {
	ACCESS_NONE,
	ACCESS_WRITE,
	ACCESS_STREAM
};
enum EAddressMode { 
	ADDRESS_WRAP, 
	ADDRESS_MIRROR, 
	ADDRESS_CLAMP 
};
enum EFilterMode { 
	FILTER_NEAREST, 
	FILTER_LINEAR 
};
enum ECompFunc {
	COMPFUNC_NEVER,
	COMPFUNC_LESS,
	COMPFUNC_EQUAL,
	COMPFUNC_LEQUAL,
	COMPFUNC_GREATER,
	COMPFUNC_NOT_EQUAL,
	COMPFUNC_GREATER_EQUAL,
	COMPFUNC_ALWAYS
};
enum ECullMode { 
	CULL_NONE, 
	CULL_BACK, 
	CULL_FRONT 
};
enum EFillMode { 
	FILL_SOLID, 
	FILL_WIREFRAME 
};
enum EBlendOp {
	BLENDOP_ADD, 
	BLENDOP_SUBTRACT, 
	BLENDOP_REVERSE_SUBTRACT, 
	BLENDOP_MIN, 
	BLENDOP_MAX
};
enum EBlendMode {
	BLEND_ZERO, 
	BLEND_ONE, 
	BLEND_SRC_COLOR, 
	BLEND_ONE_MINUS_SRC_COLOR, 
	BLEND_DST_COLOR, 
	BLEND_ONE_MINUS_DST_COLOR,
	BLEND_SRC_ALPHA, 
	BLEND_ONE_MINUS_SRC_ALPHA, 
	BLEND_DST_ALPHA, 
	BLEND_ONE_MINUS_DST_ALPHA, 
	BLEND_SRC_ALPHA_SATURATE
};
enum EDepthMode {
	DEPTH_MODE_NEVER, 
	DEPTH_MODE_LESS, 
	DEPTH_MODE_EQUAL, 
	DEPTH_MODE_LEQUAL, 
	DEPTH_MODE_GREATER, 
	DEPTH_MODE_NOTEQUAL, 
	DEPTH_MODE_GEQUAL, 
	DEPTH_MODE_ALWAYS
};
enum EPrimitiveMode {
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_LINES,
	PRIMITIVE_POINTS,
};
enum EAttribType {
	ATTRIB_POSITION,
	ATTRIB_TEXCOORD,
	ATTRIB_NORMAL,
	ATTRIB_COLOR,
};
enum EAttribFormat {
	FORMAT_FLOAT,
	FORMAT_SHORT,
	FORMAT_BYTE,
};

struct FormatDesc {
	int stream;
	EAttribType type;
	EAttribFormat format;
	int count;
};

class RenderSystem {
	struct Shader;
	struct Texture;
	struct VertexBuffer;
	struct IndexBuffer;
	struct VertexFormat;
	struct SamplerState;
	struct BlendState;
	struct DepthState;
	struct RasterizerState;
public:
	RenderSystem(HWND hWnd, int width, int height);
	~RenderSystem();

	void Clear();
	void Present();

	int CreateShader(const std::string& vertex_source, const std::string& pixel_source);
	void DestroyShader(int id);
	int CreateTexture(void* buffer, unsigned int width, unsigned int height, unsigned int format);
	void DestroyTexture(int id);
	int CreateVertexFormat(FormatDesc* desc, unsigned int count, int shader_id);
	void DestoryVertexFormat(int id);
	int CreateVertexBuffer(EBufferAccess access, void* buffer, unsigned int size);
	void DestroyVertexBuffer(int id);
	int CreateIndexBuffer(void* buffer, unsigned int size, unsigned int count);
	void DestroyIndexBuffer(int id);
	int CreateSamplerState(EAddressMode addr_u, EAddressMode addr_v, EFilterMode filter);
	void DestroySamplerState(int id);
	int CreateBlendState(bool enable, EBlendOp blend_color, EBlendMode src_color, EBlendMode dst_color, EBlendOp blend_alpha, EBlendMode src_alpha, EBlendMode dst_alpha, int mask = 0xf, bool alpha_to = false);
	void DestroyBlendState(int id);
	int CreateDepthState(bool write, bool test, EDepthMode mode);
	void DestroyDepthState(int id);
	int CreateRasterizerState(ECullMode cull, EFillMode fill = FILL_SOLID);
	void DestroyRasterizerState(int id);

	void SetShaderConstant1f(const char* name, const float constant);
	void SetShaderConstant2f(const char* name, const Vec2& constant);
	void SetShaderConstant3f(const char* name, const Vec3& constant);
	void SetShaderConstant4f(const char* name, const Vec3& constant);
	void SetShaderConstantMatrix(const char* name, const Mat4& constant);
	void SetShaderConstantRaw(const char* name, const void* data, unsigned int size);

	void SelectShader(int id);
	void SelectTexture(int id);
	void SelectVertexBuffer(int id);
	void SelectIndexBuffer(int id);
	void SelectVertexFormat(int id);
	void SelectSamplerState(int id);
	void SelectBlendState(int id);
	void SelectDepthState(int id);
	void SelectRasterizerState(int id);

	void Apply();

	void Draw(EPrimitiveMode mode, unsigned int start, unsigned int count);
	void DrawIndexed(EPrimitiveMode mode, unsigned int start, unsigned int count);

	void UpdateVertexBuffer(int id, unsigned int size, void* buffer);

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }

private:
	void ApplyShader();
	void ApplyShaderConstants();
	void ApplyTexture();
	void ApplyVertexFormat();
	void ApplyVertexBuffer();
	void ApplyIndexBuffer();
	void ApplySamplerState();
	void ApplyBlendState();
	void ApplyDepthState();
	void ApplyRasterizerState();

private:
	HWND m_hWnd;
	int m_width;
	int m_height;

	IDXGISwapChain* m_swap_chain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	ID3D11RenderTargetView* m_render_target_view;

	ID3D11Texture2D* m_depth_stencil_buffer;
	ID3D11DepthStencilView* m_depth_stencil_view;

	std::vector<Shader> m_shaders;
	std::vector<Texture> m_textures;
	std::vector<VertexBuffer> m_vertex_buffers;
	std::vector<IndexBuffer> m_index_buffers;
	std::vector<VertexFormat> m_vertex_formats;
	std::vector<SamplerState> m_sampler_states;
	std::vector<BlendState> m_blend_states;
	std::vector<DepthState> m_depth_states;
	std::vector<RasterizerState> m_rasterizer_states;

	union RenderState {
		int S[10];
		struct {
			int m_shader;
			int m_texture;
			int m_vertex_buffer;
			int m_index_buffer;
			int m_vertex_format;
			int m_sampler_state;
			int m_blend_state;
			int m_depth_state;
			int m_rasterizer_state;
			int m_topology;
		};
	};

	RenderState m_selected;
	RenderState m_current;
};
