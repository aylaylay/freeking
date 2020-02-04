#pragma once

#include "VertexBinding.h"
#include "VertexBuffer.h"
#include "Quaternion.h"
#include <vector>

namespace Freeking
{
	class Shader;

	class LineRenderer
	{
	public:

		static std::shared_ptr<LineRenderer> Debug;

		LineRenderer(std::size_t maxVertexCount);

		void DrawLine(const Vector3f& p1, const Vector3f& p2, const Vector4f& colour);
		void DrawBox(const Matrix4x4& transform, const Vector3f& mins, const Vector3f& maxs, const Vector4f& colour);
		void DrawAABBox(const Vector3f& mins, const Vector3f& maxs, const Vector4f& colour);
		void DrawAABBox(const Vector3f& position, const Vector3f& mins, const Vector3f& maxs, const Vector4f& colour);
		void DrawSphere(const Vector3f& position, float radius, int thetaSegments, int phiSegments, const Vector4f& colour);
		void DrawAxis(const Matrix4x4& transform, float length, float size);
		void DrawArrow(const Vector3f& start, const Vector3f& end, const Vector3f& up, const Vector3f& right, const float size, const Vector4f& color);

		void Flush(Matrix4x4& viewProj);
		void Clear();

		inline std::size_t GetVertexCount() const { return _vertexCount; }
		inline std::size_t GetMaxVertexCount() const { return _maxVertexCount; }

	private:

		void BufferVertex(const Vector3f& position, const Vector4f& colour);

		static const std::size_t VertexSize = 28;
		std::size_t _vertexCount;
		std::size_t _maxVertexCount;

		std::unique_ptr<VertexBuffer> _vertexBuffer;
		std::unique_ptr<VertexBinding> _vertexBinding;
		std::vector<uint8_t> _buffer;
		std::shared_ptr<Shader> _shader;
	};
}
