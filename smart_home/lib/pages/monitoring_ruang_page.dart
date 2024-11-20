import 'package:flutter/material.dart';

class MonitoringRuangPage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Monitoring Ruang'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text('Kelembapan: 60%', style: TextStyle(fontSize: 24)),
            SizedBox(height: 16),
            Text('Suhu: 24°C', style: TextStyle(fontSize: 24)),
          ],
        ),
      ),
    );
  }
}
