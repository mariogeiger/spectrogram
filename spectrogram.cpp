#include "spectrogram.h"
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <limits>

Spectrogram::Spectrogram(SpectrumAnalyser *analyser, QWidget *parent) :
    QWidget(parent), m_analyser(analyser)
{
    connect(m_analyser, SIGNAL(spectrumReady()), SLOT(moreSpectrum()));

    m_palette.setColorAt(0.0, Qt::black);
    m_palette.setColorAt(0.6, Qt::darkBlue);
    m_palette.setColorAt(0.7, Qt::magenta);
    m_palette.setColorAt(0.8, Qt::red);
    m_palette.setColorAt(0.9, Qt::yellow);
    m_palette.setColorAt(1.0, Qt::white);

    m_min = std::numeric_limits<double>::max();
    m_max = std::numeric_limits<double>::min();
}

void Spectrogram::moreSpectrum()
{
    double maxFrecency = 10000.0;
    int w = width();
    int h = qMin<int>(maxFrecency / m_analyser->frequenceUnit(), m_analyser->spectrum().size());
    QSize size(h, w); // rotate 90 degrees

    if (size == m_image.size()) {
        std::copy(m_image.bits() + m_image.bytesPerLine(), m_image.bits() + m_image.byteCount(), m_image.bits());
    } else {
        m_image = QImage(size, QImage::Format_RGB32);
        m_image.fill(Qt::white);
    }
    for (int y = 0; y < h; ++y) {
        double amp = m_analyser->spectrum()[y];
        m_image.setPixel(h-y-1, w-1, gradiantAt(amp));
    }

    update();
}

void Spectrogram::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.rotate(90.0);
    painter.scale(1.0, -1.0);
    painter.drawImage(QRect(0, 0, height(), width()), m_image);
}

QRgb Spectrogram::gradiantAt(double amp) //! a optimiser
{
    if (amp <= 0.0)
        amp = m_min;
    else
        amp = std::log10(amp);

    if (std::isinf(amp))
        return 0;

    if (amp < m_min) {
        m_min = amp;
    }
    if (amp > m_max) {
        m_max = amp;
    }

    amp = (amp - m_min) / (m_max - m_min);
    return m_palette.getColorAt(amp);
}
