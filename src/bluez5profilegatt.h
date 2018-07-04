// Copyright (c) 2018 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef BLUEZ5PROFILEGATT_H
#define BLUEZ5PROFILEGATT_H

#include <gio/gio.h>
#include <string>
#include <unordered_map>

#include <bluetooth-sil-api.h>
#include "bluez5profilebase.h"

extern "C" {
#include "freedesktop-interface.h"
#include "bluez-interface.h"
}

class Bluez5Adapter;
class GattRemoteDescriptor
{
public:
	GattRemoteDescriptor(BluezGattDescriptor1 *interface)
		: mInterface(interface) {
	}
	std::vector<unsigned char> descriptorReadValue(uint16_t offset = 0);
	bool descriptorWriteValue(const std::vector<unsigned char> &descriptorValue, uint16_t offset = 0);
	std::string parentObjectPath;
	std::string objectPath;
	BluetoothGattDescriptor descriptor;
	BluezGattDescriptor1 *mInterface;
};

class Bluez5ProfileGatt;
class GattRemoteCharacteristic
{
public:
	static void onCharacteristicPropertiesChanged(GDBusProxy *proxy, GVariant *changed_properties,
                                                  GStrv invalidated_properties, gpointer userdata);
	GattRemoteCharacteristic(BluezGattCharacteristic1 *interface, Bluez5ProfileGatt *gattProfile)
		: mInterface(interface), mGattProfile(gattProfile) {
		g_signal_connect(G_DBUS_PROXY(interface), "g-properties-changed",
						 G_CALLBACK(GattRemoteCharacteristic::onCharacteristicPropertiesChanged), this);
	}
	bool startNotify();
	bool stopNotify();
	std::vector<unsigned char> characteristicReadValue(uint16_t offset = 0);
	bool characteristicWriteValue(const std::vector<unsigned char> &characteristicValue, uint16_t offset = 0);
	std::string parentObjectPath;
	std::string objectPath;
	BluetoothGattCharacteristic characteristic;
	BluezGattCharacteristic1 *mInterface;
	Bluez5ProfileGatt *mGattProfile;
	std::vector<GattRemoteDescriptor*> gattRemoteDescriptors;
};

class GattRemoteService
{
public:
	GattRemoteService(BluezGattService1 *interface)
		: mInterface(interface) {
	}
	std::string parentObjectPath;
	std::string objectPath;
	BluetoothGattService service;
	BluezGattService1 *mInterface;
	std::vector<GattRemoteCharacteristic*> gattRemoteCharacteristics;
};

class Bluez5ProfileGatt : public Bluez5ProfileBase,
	                         public BluetoothGattProfile
{
public:
	typedef uint16_t id_type;
	Bluez5ProfileGatt(Bluez5Adapter *adapter);
	~Bluez5ProfileGatt();

	void connectGatt(const uint16_t & appId, bool autoConnection, const std::string & address, BluetoothConnectCallback callback);

	uint16_t addApplication(const BluetoothUuid &appUuid, ApplicationType type);
	bool removeApplication(uint16_t appId, ApplicationType type);
	void disconnectGatt(const uint16_t &appId, const uint16_t &connectId, const std::string &address, BluetoothResultCallback callback);
	void getProperties(const std::string &address, BluetoothPropertiesResultCallback  callback);
	void getProperty(const std::string &address, BluetoothProperty::Type type,
	                         BluetoothPropertyResultCallback callback);

	void discoverServices(BluetoothResultCallback callback);
	void discoverServices(const std::string &address, BluetoothResultCallback callback);
	void readDescriptors(const std::string &address, const BluetoothUuid& service, const BluetoothUuid &characteristic,
	                             const BluetoothUuidList &descriptors, BluetoothGattReadDescriptorsCallback callback);
	void writeDescriptor(const std::string &address, const BluetoothUuid &service, const BluetoothUuid &characteristic,
	                             const BluetoothGattDescriptor &descriptor, BluetoothResultCallback callback);
	BluetoothGattService getService(const std::string &address, const BluetoothUuid &uuid);
	BluetoothGattServiceList getServices(const std::string &address);
	void readCharacteristic(const uint16_t &connId, const BluetoothUuid& service,
									 const BluetoothUuid &characteristics,
									 BluetoothGattReadCharacteristicCallback callback);
	void writeCharacteristic(const uint16_t &connId, const BluetoothUuid& service,
									 const BluetoothGattCharacteristic &characteristic,
									 BluetoothResultCallback callback);
	void readDescriptor(const uint16_t &connId, const BluetoothUuid& service, const BluetoothUuid &characteristic,
								 const BluetoothUuid &descriptor, BluetoothGattReadDescriptorCallback callback);
	void writeDescriptor(const uint16_t &connId, const BluetoothUuid &service, const BluetoothUuid &characteristic,
								 const BluetoothGattDescriptor &descriptor, BluetoothResultCallback callback);
	void changeCharacteristicWatchStatus(const std::string &address, const BluetoothUuid &service,
												 const BluetoothUuid &characteristic, bool enabled,
												 BluetoothResultCallback callback);
	void readCharacteristic(const std::string &address, const BluetoothUuid& service,
									 const BluetoothUuid &characteristic,
									 BluetoothGattReadCharacteristicCallback callback);
	void readCharacteristics(const std::string &address, const BluetoothUuid& service,
	                                 const BluetoothUuidList &characteristics,
	                                 BluetoothGattReadCharacteristicsCallback callback);
	void writeCharacteristic(const std::string &address, const BluetoothUuid& service,
	                                 const BluetoothGattCharacteristic &characteristic,
	                                 BluetoothResultCallback callback);
	void readDescriptor(const std::string &address, const BluetoothUuid& service, const BluetoothUuid &characteristic,
								 const BluetoothUuid &descriptor, BluetoothGattReadDescriptorCallback callback);
	uint16_t getConnectId(const std::string &address);
	std::string getAddress(const uint16_t &connId);
	GattRemoteService* findService(const std::string &address, const BluetoothUuid& service);
	GattRemoteCharacteristic* findCharacteristic(GattRemoteService* service, const BluetoothUuid &characteristic);
	GattRemoteDescriptor* findDescriptor(GattRemoteCharacteristic* characteristic, const BluetoothUuid &descriptor);
	void addService(uint16_t appId, const BluetoothGattService &service, BluetoothGattAddCallback callback);
	void removeService(uint16_t appId, uint16_t serviceId, BluetoothResultCallback callback);

	void addDescriptor(uint16_t appId, uint16_t serviceId, const BluetoothGattDescriptor &descriptor, BluetoothGattAddCallback callback);
	void addCharacteristic(uint16_t appId, uint16_t serviceId, const BluetoothGattCharacteristic &characteristic, BluetoothGattAddCallback callback);
	void startService(uint16_t serviceId, BluetoothGattTransportMode mode, BluetoothResultCallback callback);
	void startService(uint16_t appId, uint16_t serviceId, BluetoothGattTransportMode mode, BluetoothResultCallback callback);
	void onCharacteristicPropertiesChanged(GattRemoteCharacteristic* characteristic, GVariant *changed_properties);

	static gboolean handleRelease(BluezGattProfile1 *proxy, GDBusMethodInvocation *invocation, gpointer user_data);
	static void handleBusAcquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

private:

	class Bluez5GattLocalDescriptor;
	class Bluez5GattLocalCharacteristic;
	class Bluez5GattLocalService;

	typedef std::unordered_map <id_type, std::unique_ptr<Bluez5GattLocalDescriptor>> GattLocalDescriptorsMap;
	typedef std::unordered_map <id_type, std::unique_ptr<Bluez5GattLocalCharacteristic>> GattLocalCharacteristicsMap;
	typedef std::unordered_map<id_type, std::unique_ptr <Bluez5GattLocalService>> GattLocalServiceMap;

	class Bluez5GattLocalDescriptor
	{
		public:
			Bluez5GattLocalDescriptor(GDBusObject *object):
			mDescObject(object)
			{
			}
			GDBusObject *mDescObject;
			BluezGattDescriptor1 *mInterface;
	};

	class Bluez5GattLocalCharacteristic
	{
		public:
			Bluez5GattLocalCharacteristic(GDBusObject *object):
			mCharObject(object)
			{
			}
			GDBusObject *mCharObject;
			BluezGattCharacteristic1 *mInterface;
			GattLocalDescriptorsMap mDescriptors;
	};

	class Bluez5GattLocalService
	{
		public:
			Bluez5GattLocalService(GDBusObject *object):
			mServiceObject(object)
			{
			}
			GDBusObject *mServiceObject;
			GattLocalCharacteristicsMap mCharacteristics;
			BluezGattService1 *mServiceInterface;
			uint16_t serviceId;
	};

	class BluezGattLocalApplication
	{
		public:
			GattLocalServiceMap mGattLocalServices;
	};

	void setGdbusConnection(GDBusConnection *conn) { mConn = conn; }
	void registerLocalApplication(BluetoothResultCallback callback, const std::string &objPath, bool unRegisterFirst);
	void createObjectManagers();
	void removeLocalServices(Bluez5GattLocalService *service);
	void removeLocalCharacteristics(Bluez5GattLocalService *service);
	void removeLocalDescriptors(Bluez5GattLocalCharacteristic *characteristic);
	GattLocalDescriptorsMap* getLocalDescriptorList(uint16_t appId, uint16_t serviceId, uint16_t charId);
	void updatePropertyFlags(const BluetoothGattCharacteristic &characteristic, const char **propertyflags);
	void updatePermissionFlags(const BluetoothGattDescriptor &descriptor, const char **permissionflags);

	id_type nextAppId();
	id_type nextServiceId();
	id_type nextCharId();
	id_type nextDescId();

private:
	void registerSignalHandlers();

	void addRemoteServiceToDevice(GattRemoteService* gattService);
	void createRemoteGattService(const std::string &serviceObjectPath);
	void removeRemoteGattService(const std::string &serviceObjectPath);

	void addRemoteCharacteristicToService(GattRemoteCharacteristic* gattCharacteristic);
	void createRemoteGattCharacteristic(const std::string &characteristicObjectPath);
	void removeRemoteGattCharacteristic(const std::string &characteristicObjectPath);

	void addRemoteDescriptorToCharacteristic(GattRemoteDescriptor* gattDescriptor);
	void createRemoteGattDescriptor(const std::string &descriptorObjectPath);
	void removeRemoteGattDescriptor(const std::string &descriptorObjectPath);

	GattRemoteService* getRemoteGattService(std::string& serviceObjectPath);
	void updateRemoteDeviceServices();

	static void handleObjectAdded(GDBusObjectManager *objectManager, GDBusObject *object,
									void *user_data);
	static void handleObjectRemoved(GDBusObjectManager *objectManager, GDBusObject *object,
									void *user_data);

	guint mBusId;
	id_type mLastCharId;
	GDBusConnection *mConn;
	Bluez5Adapter *mAdapter;
	GDBusObjectManagerServer *mObjectManagerGattServer;
	GDBusObjectManager *mObjectManager;

	typedef std::vector<GattRemoteService*> GattServiceList;
	std::unordered_map<id_type, std::string> mConnectedDevices;
	std::unordered_map<id_type, std::unique_ptr <BluezGattLocalApplication>> mGattLocalApplications;
	std::unordered_map<std::string, GattServiceList> mDeviceServicesMap;
	std::unordered_map<std::string, BluetoothGattServiceList> mRemoteDeviceServicesMap;
};

#endif // BLUEZ5PROFILEGATT_H