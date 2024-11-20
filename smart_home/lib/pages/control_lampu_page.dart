import 'package:flutter/material.dart';
import 'esp_service.dart'; // Pastikan EspService diimport dengan benar

class ControlLampuPage extends StatefulWidget {
  @override
  _ControlLampuPageState createState() => _ControlLampuPageState();
}

class _ControlLampuPageState extends State<ControlLampuPage> {
  final EspService espService =
      EspService("http://192.168.43.233"); // Ganti dengan IP ESP8266 Anda
  bool _lampu1On = false;
  bool _lampu2On = false;
  double _brightnessLampu1 = 0.5;
  double _brightnessLampu2 = 0.5;
  bool _lampuOtomatis = false;

  void _turnOnAllLights() async {
    setState(() {
      _lampu1On = true;
      _lampu2On = true;
      _brightnessLampu1 = 1.0;
      _brightnessLampu2 = 1.0;
    });
    await espService.sendCommand('lamp1/on');
    await espService.sendCommand('lamp2/on');
  }

  void _turnOffAllLights() async {
    setState(() {
      _lampu1On = false;
      _lampu2On = false;
      _brightnessLampu1 = 0.0;
      _brightnessLampu2 = 0.0;
    });
    await espService.sendCommand('lamp1/off');
    await espService.sendCommand('lamp2/off');
  }

  void _toggleAutomaticLight() async {
    if (_lampuOtomatis) {
      setState(() {
        if (_brightnessLampu1 < 0.3) _lampu1On = true;
        if (_brightnessLampu2 < 0.3) _lampu2On = true;
        if (_brightnessLampu1 >= 0.3) _lampu1On = false;
        if (_brightnessLampu2 >= 0.3) _lampu2On = false;
      });
      if (_lampu1On)
        await espService.sendCommand('lamp1/on');
      else
        await espService.sendCommand('lamp1/off');

      if (_lampu2On)
        await espService.sendCommand('lamp2/on');
      else
        await espService.sendCommand('lamp2/off');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Control Lampu'),
      ),
      body: SingleChildScrollView(
        padding: EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            // Kontrol Lampu 1
            _buildLampControl(
              lampuOn: _lampu1On,
              brightness: _brightnessLampu1,
              onToggle: (value) async {
                setState(() {
                  _lampu1On = value;
                });
                if (_lampu1On) {
                  await espService.sendCommand('lamp1/on');
                } else {
                  await espService.sendCommand('lamp1/off');
                }
              },
              onBrightnessChange: (value) async {
                setState(() {
                  _brightnessLampu1 = value;
                  _toggleAutomaticLight();
                });
                await espService.sendCommand('lamp1/brightness/$value');
              },
              title: 'Lampu 1',
              color: Colors.yellow,
            ),
            SizedBox(height: 20),
            // Kontrol Lampu 2
            _buildLampControl(
              lampuOn: _lampu2On,
              brightness: _brightnessLampu2,
              onToggle: (value) async {
                setState(() {
                  _lampu2On = value;
                });
                if (_lampu2On) {
                  await espService.sendCommand('lamp2/on');
                } else {
                  await espService.sendCommand('lamp2/off');
                }
              },
              onBrightnessChange: (value) async {
                setState(() {
                  _brightnessLampu2 = value;
                  _toggleAutomaticLight();
                });
                await espService.sendCommand('lamp2/brightness/$value');
              },
              title: 'Lampu 2',
              color: Colors.orange,
            ),
            SizedBox(height: 20),
            // Lampu Otomatis
            SwitchListTile(
              title: Text('Lampu Otomatis'),
              value: _lampuOtomatis,
              onChanged: (bool value) {
                setState(() {
                  _lampuOtomatis = value;
                  _toggleAutomaticLight();
                });
              },
            ),
            SizedBox(height: 20),
            // Tombol Matikan dan Nyalakan Semua Lampu
            Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                ElevatedButton(
                  onPressed: _turnOnAllLights,
                  child: Text('Nyalakan Semua Lampu'),
                  style:
                      ElevatedButton.styleFrom(backgroundColor: Colors.green),
                ),
                SizedBox(height: 10),
                ElevatedButton(
                  onPressed: _turnOffAllLights,
                  child: Text('Matikan Semua Lampu'),
                  style: ElevatedButton.styleFrom(backgroundColor: Colors.red),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildLampControl({
    required bool lampuOn,
    required double brightness,
    required ValueChanged<bool> onToggle,
    required ValueChanged<double> onBrightnessChange,
    required String title,
    required Color color,
  }) {
    return AnimatedContainer(
      duration: Duration(milliseconds: 500),
      decoration: BoxDecoration(
        color: lampuOn ? color.withOpacity(0.3) : Colors.grey[300],
        borderRadius: BorderRadius.circular(15),
      ),
      padding: EdgeInsets.all(16.0),
      child: Column(
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                title,
                style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
              ),
              Switch(
                value: lampuOn,
                onChanged: onToggle,
              ),
            ],
          ),
          AnimatedOpacity(
            opacity: brightness,
            duration: Duration(milliseconds: 300),
            child: Icon(
              Icons.lightbulb,
              color: lampuOn ? color : Colors.grey,
              size: 50,
            ),
          ),
          Slider(
            value: brightness,
            min: 0.0,
            max: 1.0,
            divisions: 10,
            label: (brightness * 100).toInt().toString() + '%',
            onChanged: lampuOn ? onBrightnessChange : null,
          ),
        ],
      ),
    );
  }
}
