/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2016 John Preston, https://desktop.telegram.org
*/
#pragma once

#include "ui/style/style_core_color.h"

namespace style {
namespace internal {

class IconMask {
public:
	template <int N>
	IconMask(const uchar (&data)[N]) : _data(data), _size(N) {
		static_assert(N > 0, "invalid image data");
	}

	const uchar *data() const {
		return _data;
	}
	int size() const {
		return _size;
	}

private:
	const uchar *_data;
	const int _size;

};

class MonoIcon {
public:
	MonoIcon() = default;
	MonoIcon(const IconMask *mask, const Color &color, QPoint offset);

	void reset() const;
	int width() const;
	int height() const;
	QSize size() const;

	QPoint offset() const;

	void paint(QPainter &p, const QPoint &pos, int outerw) const;
	void fill(QPainter &p, const QRect &rect) const;
	void paint(QPainter &p, const QPoint &pos, int outerw, QColor colorOverride) const;
	void fill(QPainter &p, const QRect &rect, QColor colorOverride) const;

	~MonoIcon() {
	}

private:
	void ensureLoaded() const;
	void createCachedPixmap() const;

	const IconMask *_mask = nullptr;
	Color _color;
	QPoint _offset = { 0, 0 };
	mutable QImage _maskImage;
	mutable QPixmap _pixmap; // for pixmaps
	mutable QSize _size; // for rects

};

class IconData {
public:
	template <typename ...MonoIcons>
	IconData(const MonoIcons &...icons) {
		created();
		_parts.reserve(sizeof...(MonoIcons));
		addIcons(icons...);
	}

	void reset() {
		for_const (auto &part, _parts) {
			part.reset();
		}
	}
	bool empty() const {
		return _parts.empty();
	}

	void paint(QPainter &p, const QPoint &pos, int outerw) const {
		for_const (auto &part, _parts) {
			part.paint(p, pos, outerw);
		}
	}
	void fill(QPainter &p, const QRect &rect) const;

	void paint(QPainter &p, const QPoint &pos, int outerw, QColor colorOverride) const {
		for_const (auto &part, _parts) {
			part.paint(p, pos, outerw, colorOverride);
		}
	}
	void fill(QPainter &p, const QRect &rect, QColor colorOverride) const;

	int width() const;
	int height() const;

private:
	void created();

	template <typename ... MonoIcons>
	void addIcons() {
	}

	template <typename ... MonoIcons>
	void addIcons(const MonoIcon &icon, const MonoIcons&... icons) {
		_parts.push_back(icon);
		addIcons(icons...);
	}

	QVector<MonoIcon> _parts;
	mutable int _width = -1;
	mutable int _height = -1;

};

class Icon {
public:
	Icon(Qt::Initialization) {
	}

	template <typename ... MonoIcons>
	Icon(const MonoIcons&... icons) : _data(new IconData(icons...)), _owner(true) {
	}
	Icon(const Icon &other) : _data(other._data) {
	}
	Icon(Icon &&other) : _data(base::take(other._data)), _owner(base::take(_owner)) {
	}
	Icon &operator=(const Icon &other) {
		t_assert(!_owner);
		_data = other._data;
		_owner = false;
		return *this;
	}
	Icon &operator=(Icon &&other) {
		t_assert(!_owner);
		_data = base::take(other._data);
		_owner = base::take(other._owner);
		return *this;
	}

	bool empty() const {
		return _data->empty();
	}

	void paint(QPainter &p, const QPoint &pos, int outerw) const {
		return _data->paint(p, pos, outerw);
	}
	void paint(QPainter &p, int x, int y, int outerw) const {
		return _data->paint(p, QPoint(x, y), outerw);
	}
	void paintInCenter(QPainter &p, const QRect &outer) const {
		return _data->paint(p, QPoint(outer.x() + (outer.width() - width()) / 2, outer.y() + (outer.height() - height()) / 2), outer.x() * 2 + outer.width());
	}
	void fill(QPainter &p, const QRect &rect) const {
		return _data->fill(p, rect);
	}

	void paint(QPainter &p, const QPoint &pos, int outerw, QColor colorOverride) const {
		return _data->paint(p, pos, outerw, colorOverride);
	}
	void paint(QPainter &p, int x, int y, int outerw, QColor colorOverride) const {
		return _data->paint(p, QPoint(x, y), outerw, colorOverride);
	}
	void paintInCenter(QPainter &p, const QRect &outer, QColor colorOverride) const {
		return _data->paint(p, QPoint(outer.x() + (outer.width() - width()) / 2, outer.y() + (outer.height() - height()) / 2), outer.x() * 2 + outer.width(), colorOverride);
	}
	void fill(QPainter &p, const QRect &rect, QColor colorOverride) const {
		return _data->fill(p, rect, colorOverride);
	}

	int width() const {
		return _data->width();
	}
	int height() const {
		return _data->height();
	}
	QSize size() const {
		return QSize(width(), height());
	}

	~Icon() {
		if (auto data = base::take(_data)) {
			if (_owner) {
				delete data;
			}
		}
	}

private:
	IconData *_data = nullptr;
	bool _owner = false;

};

void resetIcons();
void destroyIcons();

} // namespace internal
} // namespace style
