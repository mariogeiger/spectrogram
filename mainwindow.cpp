#include "mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QAudioDeviceInfo audioDevice = QAudioDeviceInfo::defaultInputDevice();
    m_format = audioDevice.preferredFormat();
    m_format.setByteOrder((QAudioFormat::Endian)QSysInfo::ByteOrder);
    m_format.setSampleType(QAudioFormat::SignedInt);
    m_format.setSampleSize(16);
    m_format.setChannelCount(1);
    m_format.setSampleRate(audioDevice.supportedSampleRates().last());
    qDebug() << m_format;
    if (!audioDevice.isFormatSupported(m_format)) {
        qDebug("Format not supported");
    }
    m_audioinput = new QAudioInput(audioDevice, m_format, this);

    m_spectrumanalyser = new SpectrumAnalyser(this);
    m_spectrumanalyser->setAudioFormat(m_format);
    m_spectrumanalyser->setParameters(m_format.sampleRate() / 10, m_format.sampleRate() / 40);
    m_spectrumanalyser->open(QIODevice::WriteOnly);
    m_audioinput->start(m_spectrumanalyser);

    m_spectrogram = new Spectrogram(m_spectrumanalyser, this);
    setCentralWidget(m_spectrogram);
}

MainWindow::~MainWindow()
{
    
}
