#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <QWidget>
#include <QImage>
#include "palette.h"
#include "spectrumanalyser.h"

class Spectrogram : public QWidget
{
    Q_OBJECT
public:
    explicit Spectrogram(SpectrumAnalyser* analyser, QWidget *parent = 0);
    
private slots:
    void moreSpectrum();

private:
    void paintEvent(QPaintEvent *);
    QRgb gradiantAt(double amp);

    SpectrumAnalyser* m_analyser;
    QList<QVector<double> > m_specs;
    Palette m_palette;

    QImage m_image;

    double m_min, m_max;
};

#endif // SPECTROGRAM_H
