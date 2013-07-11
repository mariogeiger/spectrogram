#include "spectrumanalyser.h"
#include <QDebug>
#include <algorithm>

SpectrumAnalyser::SpectrumAnalyser(QObject *parent) :
    QIODevice(parent)
{
    setParameters(100, 10);
}

SpectrumAnalyser::~SpectrumAnalyser()
{
    fftw_destroy_plan(m_plan);
}

qint64 SpectrumAnalyser::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);
    return 0;
}

qint64 SpectrumAnalyser::writeData(const char *data, qint64 len)
{
    //qDebug() << len;

    Q_ASSERT((QSysInfo::Endian)m_audioFormat.byteOrder() == QSysInfo::ByteOrder);
    Q_ASSERT(m_audioFormat.sampleType() == QAudioFormat::SignedInt);
    Q_ASSERT(m_audioFormat.channelCount() == 1);
    Q_ASSERT(m_audioFormat.sampleSize() == 16);

    int samplesReady = len / 2; // 16bit = 2bytes
    const qint16* ptr = reinterpret_cast<const qint16*>(data);

    while (samplesReady > 0) {
        const int canReadNow = qMin(samplesReady, m_needed);
        for (int i = 0; i < canReadNow; ++i) {
            m_fftin[m_nFFT - m_needed] = *ptr / 32768.0;
            --m_needed;
            --samplesReady;
            ++ptr;
        }
        if (m_needed == 0) {
            runFFT();
            m_needed = m_overlap;
            std::copy(m_fftin.begin() + m_overlap, m_fftin.end(), m_fftin.begin());
        }
    }

    return len - samplesReady;
}

// en théorie l'oreille humaine est censée percevoir des fréquences comprises entre 20 Hz et 20 000 Hz
void SpectrumAnalyser::runFFT()
{
    fftw_execute(m_plan);
    double peakFrec = 0.0;
    double peakAmplitude = 0.0;
    double x;
    for (int i = 0; i < m_fftout.size(); ++i) {
        x = std::norm(m_fftout[i] / double(m_nFFT)); // The norm value of a complex number is the squared magnitude
        if (x > peakAmplitude) {
            peakFrec = m_frequenceUnit * i;
            peakAmplitude = x;
        }
        m_spectrum[i] = x;
    }
    qDebug() << peakFrec << "Hz +-" << m_frequenceUnit << " - amplitude:" << peakAmplitude;
    emit spectrumReady();
}

QAudioFormat SpectrumAnalyser::audioFormat() const
{
    return m_audioFormat;
}

void SpectrumAnalyser::setAudioFormat(const QAudioFormat &audioFormat)
{
    m_audioFormat = audioFormat;
    m_frequenceUnit = m_audioFormat.sampleRate() / m_nFFT;
}

void SpectrumAnalyser::setParameters(int nfft, int overlap)
{
    Q_ASSERT(overlap <= nfft);
    m_nFFT = nfft;
    m_overlap = overlap;

    m_fftin.fill(0.0, m_nFFT);
    m_needed = overlap;

    m_fftout.resize(m_nFFT / 2 + 1);
    m_spectrum.resize(m_fftout.size());

    fftw_destroy_plan(m_plan);
    m_plan = fftw_plan_dft_r2c_1d(m_nFFT, m_fftin.data(), reinterpret_cast<fftw_complex*>(m_fftout.data()), 0);

    m_frequenceUnit = m_audioFormat.sampleRate() / m_nFFT;
}

const QVector<double> &SpectrumAnalyser::spectrum() const
{
    return m_spectrum;
}

double SpectrumAnalyser::frequenceUnit() const
{
    return m_frequenceUnit;
}
