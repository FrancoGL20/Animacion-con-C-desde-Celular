import 'dart:async';
import 'dart:convert';
import 'dart:core';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter_sensors/flutter_sensors.dart';

void main() => runApp(const MyApp());

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  // ignore: library_private_types_in_public_api
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  bool _accelAvailable = false;
  bool _gyroAvailable = false;
  List<double> _accelData = List.filled(3, 0.0);
  List<double> _gyroData = List.filled(3, 0.0);
  StreamSubscription? _accelSubscription;
  StreamSubscription? _gyroSubscription;
  final StreamController<String> _dataStreamController =
      StreamController<String>();

  @override
  void initState() {
    _checkAccelerometerStatus();
    _checkGyroscopeStatus();
    super.initState();
  }

  @override
  void dispose() {
    _stopAccelerometer();
    _stopGyroscope();
    super.dispose();
  }

  void _checkAccelerometerStatus() async {
    await SensorManager()
        .isSensorAvailable(Sensors.ACCELEROMETER)
        .then((result) {
      setState(() {
        _accelAvailable = result;
      });
    });
  }

  Future<void> _startAccelerometer() async {
    if (_accelSubscription != null) return;
    if (_accelAvailable) {
      final stream = await SensorManager().sensorUpdates(
        sensorId: Sensors.ACCELEROMETER,
        interval: Sensors.SENSOR_DELAY_FASTEST,
      );
      _accelSubscription = stream.listen((sensorEvent) {
        setState(() {
          _accelData = sensorEvent.data;
        });
      });
    }
  }

  void _stopAccelerometer() {
    if (_accelSubscription == null) return;
    _accelSubscription?.cancel();
    _accelSubscription = null;
  }

  void _checkGyroscopeStatus() async {
    await SensorManager().isSensorAvailable(Sensors.GYROSCOPE).then((result) {
      setState(() {
        _gyroAvailable = result;
      });
    });
  }

  void startSendingData() async {
    final socket = await Socket.connect("192.168.10.144", 11510);
    _dataStreamController.stream.listen((data) {
      socket.write({'data': data});
      socket.flush();
    });
    Timer.periodic(const Duration(milliseconds: 140), (timer) {
      _dataStreamController.add(json.encode({
        'accelX': _accelData[0],
        'accelY': _accelData[1],
        'accelZ': _accelData[2]

        // 'gyroX': _gyroData[0],
        // 'gyroY': _gyroData[1],
        // 'gyroZ': _gyroData[2]
      }));
    });
  }

  void stopSendingData() {
    _dataStreamController.close();
  }

  Future<void> _startGyroscope() async {
    if (_gyroSubscription != null) return;
    if (_gyroAvailable) {
      final stream =
          await SensorManager().sensorUpdates(sensorId: Sensors.GYROSCOPE);
      _gyroSubscription = stream.listen((sensorEvent) {
        setState(() {
          _gyroData = sensorEvent.data;
        });
      });
    }
  }

  void _stopGyroscope() {
    if (_gyroSubscription == null) return;
    _gyroSubscription?.cancel();
    _gyroSubscription = null;
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Sensors'),
        ),
        body: Container(
          padding: const EdgeInsets.all(16.0),
          alignment: AlignmentDirectional.topCenter,
          child: Column(
            children: <Widget>[
              const Text(
                "Accelerometer Test",
                textAlign: TextAlign.center,
              ),
              Text(
                "Accelerometer Enabled: $_accelAvailable",
                textAlign: TextAlign.center,
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              Text(
                "[0](X) = ${_accelData[0]}",
                textAlign: TextAlign.center,
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              Text(
                "[1](Y) = ${_accelData[1]}",
                textAlign: TextAlign.center,
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              Text(
                "[2](Z) = ${_accelData[2]}",
                textAlign: TextAlign.center,
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  MaterialButton(
                    color: Colors.green,
                    onPressed:
                        _accelAvailable ? () => _startAccelerometer() : null,
                    child: const Text("Start"),
                  ),
                  const Padding(
                    padding: EdgeInsets.all(8.0),
                  ),
                  MaterialButton(
                    color: Colors.red,
                    onPressed:
                        _accelAvailable ? () => _stopAccelerometer() : null,
                    child: const Text("Stop"),
                  ),
                ],
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              const Text(
                "Gyroscope Test",
                textAlign: TextAlign.center,
              ),
              Text(
                "Gyroscope Enabled: $_gyroAvailable",
                textAlign: TextAlign.center,
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              Text(
                "[0](X) = ${_gyroData[0]}",
                textAlign: TextAlign.center,
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              Text(
                "[1](Y) = ${_gyroData[1]}",
                textAlign: TextAlign.center,
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              Text(
                "[2](Z) = ${_gyroData[2]}",
                textAlign: TextAlign.center,
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  MaterialButton(
                    color: Colors.green,
                    onPressed: _gyroAvailable ? () => _startGyroscope() : null,
                    child: const Text("Start"),
                  ),
                  const Padding(
                    padding: EdgeInsets.all(8.0),
                  ),
                  MaterialButton(
                    color: Colors.red,
                    onPressed: _gyroAvailable ? () => _stopGyroscope() : null,
                    child: const Text("Stop"),
                  ),
                ],
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              MaterialButton(
                color: Colors.green,
                onPressed: () => startSendingData(),
                child: const Text("Start Sending Data"),
              ),
              const Padding(padding: EdgeInsets.only(top: 16.0)),
              MaterialButton(
                color: Colors.red,
                onPressed: () => stopSendingData(),
                child: const Text("Stop Sending Data"),
              ),
            ],
          ),
        ),
      ),
      debugShowCheckedModeBanner: false, // ? Remove debug banner from top right
    );
  }
}
