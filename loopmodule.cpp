#include "loopmodule.h"

LoopModule::LoopModule(QObject *parent)
    : QObject(parent), m_channelCount(1) // Default to 1 channel
      ,
      m_loopMode(LoopMode::ClassB) // Default to Class B
      ,
      m_isInitialized(false), m_isMappingSupported(false) {}

LoopModule::~LoopModule() {}

int LoopModule::getChannelCount() const { return m_channelCount; }

void LoopModule::setChannelCount(int count) {
  if (m_channelCount != count) {
    m_channelCount = count;
    emit dataChanged();
  }
}

LoopMode LoopModule::getLoopMode() const { return m_loopMode; }

void LoopModule::setLoopMode(LoopMode mode) {
  if (m_loopMode != mode) {
    m_loopMode = mode;
    emit dataChanged();
  }
}

bool LoopModule::isInitialized() const { return m_isInitialized; }

void LoopModule::setInitialized(bool initialized) {
  if (m_isInitialized != initialized) {
    m_isInitialized = initialized;
    emit dataChanged();
  }
}

bool LoopModule::isMappingSupported() const { return m_isMappingSupported; }

void LoopModule::setMappingSupported(bool supported) {
  if (m_isMappingSupported != supported) {
    m_isMappingSupported = supported;
    emit dataChanged();
  }
}

QList<LoopDevice> LoopModule::getDevices(int channelIndex) const {
  return m_devices.value(channelIndex);
}

void LoopModule::setDevices(int channelIndex,
                            const QList<LoopDevice> &devices) {
  m_devices[channelIndex] = devices;
  emit dataChanged();
}

void LoopModule::addDevice(int channelIndex, const LoopDevice &device) {
  m_devices[channelIndex].append(device);
  emit dataChanged();
}

void LoopModule::removeDevice(int channelIndex, int deviceIndex) {
  if (m_devices.contains(channelIndex)) {
    QList<LoopDevice> &list = m_devices[channelIndex];
    if (deviceIndex >= 0 && deviceIndex < list.size()) {
      list.removeAt(deviceIndex);
      emit dataChanged();
    }
  }
}

void LoopModule::updateDevice(int channelIndex, int deviceIndex,
                              const LoopDevice &device) {
  if (m_devices.contains(channelIndex)) {
    QList<LoopDevice> &list = m_devices[channelIndex];
    if (deviceIndex >= 0 && deviceIndex < list.size()) {
      list[deviceIndex] = device;
      emit dataChanged();
    }
  }
}
