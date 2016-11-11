/*
 * Copyright (C) 2013-2015 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mirbuffersgtexture.h"

// Mir
#include <mir/graphics/buffer.h>
#include <mir/geometry/size.h>
#include <mir/renderer/gl/texture_source.h>

namespace mg = mir::geometry;
namespace mrg = mir::renderer::gl;

MirBufferSGTexture::MirBufferSGTexture()
    : QSGTexture()
    , m_width(0)
    , m_height(0)
    , m_textureId(0)
{
    glGenTextures(1, &m_textureId);

    setFiltering(QSGTexture::Linear);
    setHorizontalWrapMode(QSGTexture::ClampToEdge);
    setVerticalWrapMode(QSGTexture::ClampToEdge);
}

MirBufferSGTexture::~MirBufferSGTexture()
{
    if (m_textureId) {
        glDeleteTextures(1, &m_textureId);
    }
}

void MirBufferSGTexture::freeBuffer()
{
    m_mirBuffer.reset();
    m_width = 0;
    m_height = 0;
}

void MirBufferSGTexture::setBuffer(const std::shared_ptr<mir::graphics::Buffer>& buffer)
{
    m_mirBuffer = buffer;
    mg::Size size = buffer->size();
    m_height = size.height.as_int();
    m_width = size.width.as_int();
}

bool MirBufferSGTexture::hasBuffer() const
{
    return !!m_mirBuffer;
}

int MirBufferSGTexture::textureId() const
{
    return m_textureId;
}

QSize MirBufferSGTexture::textureSize() const
{
    return QSize(m_width, m_height);
}

bool MirBufferSGTexture::hasAlphaChannel() const
{
    if (hasBuffer()) {
        return m_mirBuffer->pixel_format() == mir_pixel_format_abgr_8888
            || m_mirBuffer->pixel_format() == mir_pixel_format_argb_8888;
    } else {
        return false;
    }
}

void MirBufferSGTexture::bind()
{
    Q_ASSERT(hasBuffer());
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    updateBindOptions(true/* force */);

    auto const texture_source =
        dynamic_cast<mrg::TextureSource*>(m_mirBuffer->native_buffer_base());
    if (!texture_source)
        throw std::logic_error("Buffer does not support GL rendering");

    texture_source->gl_bind_to_texture();
}
