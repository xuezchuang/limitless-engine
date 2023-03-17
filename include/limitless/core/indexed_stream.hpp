#include <limitless/core/vertex_stream.hpp>

namespace Limitless {
    template <typename Vertex>
    class IndexedVertexStream : public VertexStream<Vertex> {
    protected:
        using index_type = std::uint32_t;
        std::vector<index_type> indices;
        std::shared_ptr<Buffer> indices_buffer;

        void initialize() {
            BufferBuilder builder;
            builder.setTarget(Buffer::Type::Element)
                    .setData(indices.data())
                    .setDataSize(indices.size() * sizeof(index_type));

            switch (this->usage) {
                case VertexStreamUsage::Static:
                    builder.setUsage(Buffer::Storage::Static)
                            .setAccess(Buffer::ImmutableAccess::None);
                    break;
                case VertexStreamUsage::Dynamic:
                    builder.setUsage(Buffer::Usage::DynamicDraw)
                            .setAccess(Buffer::MutableAccess::WriteOrphaning);
                    break;
                case VertexStreamUsage::Stream:
                    //TODO: should be x3 vertex array && x3 buffer
                    builder.setUsage(Buffer::Storage::DynamicCoherentWrite)
                            .setAccess(Buffer::ImmutableAccess::WriteCoherent);
                    break;
            }

            indices_buffer = builder.build();

            this->vertex_array.setElementBuffer(indices_buffer);
        };
    public:
        IndexedVertexStream(std::vector<Vertex>&& vertices, std::vector<index_type>&& _indices, VertexStreamUsage usage, VertexStreamDraw draw) noexcept
            : VertexStream<Vertex>(std::move(vertices), usage, draw)
            , indices{std::move(_indices)} {
            initialize();
        }

        void draw(VertexStreamDraw draw_mode) noexcept override {
            if (this->stream.empty()) {
                return;
            }

            this->vertex_array.bind();

            glDrawElements(static_cast<GLenum>(draw_mode), (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);

            this->vertex_buffer->fence();
            indices_buffer->fence();
        }

        void draw_instanced(VertexStreamDraw mode, std::size_t count) noexcept override 
        {
            if (this->stream.empty())
            {
                return;
            }

            this->vertex_array.bind();

            glDrawElementsInstanced(static_cast<GLenum>(mode), (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr, (GLsizei)count);

            this->vertex_buffer->fence();
            indices_buffer->fence();
        }

        void map() {
            const auto size = indices.size() * sizeof(index_type);

            if (size > indices_buffer->getSize()) {
                indices_buffer->resize(size);
            }

            indices_buffer->mapData(indices.data(), size);
        }

        IndexedVertexStream& operator+(const IndexedVertexStream& rhs) {
            this->operator+(static_cast<const VertexStream<Vertex>&>(rhs));

            auto cp = rhs.indices;

            std::transform(cp.begin(), cp.end(), cp.begin(), std::bind2nd(std::plus(), this->stream.size()));

            indices.insert(indices.begin(), cp.begin(), cp.end());

            map();

            return *this;
        }

        auto& getIndices() noexcept { return indices; }
        [[nodiscard]] const auto& getIndices() const noexcept { return indices; }
    };
}
