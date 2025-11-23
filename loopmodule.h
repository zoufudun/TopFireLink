#ifndef LOOPMODULE_H
#define LOOPMODULE_H

#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>


// Loop Device Node Structure
struct LoopDevice {
  QString type; // Device Type (e.g., Smoke/Heat Detector, Manual Call Point)
  QString serialNumber;    // Device Serial Number
  int address;             // Device Address
  QString personalityCode; // Personality Code
  int panelNumber;         // Panel Number
  int cardNumber;          // Card Number
  QString description;     // Device Description
  QString identifier;      // Device Identifier
  QString variableName;    // Variable Name

  // Default constructor
  LoopDevice() : address(0), panelNumber(0), cardNumber(0) {}
};

// Loop Mode Enum
enum class LoopMode { ClassA, ClassB, ClassA_Plus_B };

class LoopModule : public QObject {
  Q_OBJECT

public:
  explicit LoopModule(QObject *parent = nullptr);
  ~LoopModule();

  // Channel Count
  int getChannelCount() const;
  void setChannelCount(int count);

  // Loop Mode
  LoopMode getLoopMode() const;
  void setLoopMode(LoopMode mode);

  // Initialization and Mapping
  bool isInitialized() const;
  void setInitialized(bool initialized);

  bool isMappingSupported() const;
  void setMappingSupported(bool supported);

  // Device Management
  QList<LoopDevice> getDevices(int channelIndex) const;
  void setDevices(int channelIndex, const QList<LoopDevice> &devices);
  void addDevice(int channelIndex, const LoopDevice &device);
  void removeDevice(int channelIndex, int deviceIndex);
  void updateDevice(int channelIndex, int deviceIndex,
                    const LoopDevice &device);

signals:
  void dataChanged();

private:
  int m_channelCount;
  LoopMode m_loopMode;
  bool m_isInitialized;
  bool m_isMappingSupported;

  // Map channel index to list of devices
  QMap<int, QList<LoopDevice>> m_devices;
};

#endif // LOOPMODULE_H
