import 'package:flutter/material.dart';

class ControlGerbangPage extends StatefulWidget {
  @override
  _ControlGerbangPageState createState() => _ControlGerbangPageState();
}

class _ControlGerbangPageState extends State<ControlGerbangPage> with TickerProviderStateMixin {
  bool _isGateOpen = false;
  late AnimationController _controller;
  late Animation<double> _offsetAnimation;

  @override
  void initState() {
    super.initState();
    // Initialize the animation controller for gate opening/closing
    _controller = AnimationController(
      duration: Duration(seconds: 1),
      vsync: this,
    );

    // Define the offset animation (translation)
    _offsetAnimation = Tween<double>(begin: 0, end: 50).animate(_controller);
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  void _toggleGate(bool value) {
    setState(() {
      _isGateOpen = value;
      if (_isGateOpen) {
        _controller.forward(); // Slide the gate open
      } else {
        _controller.reverse(); // Slide the gate closed
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Control Gerbang'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            // Animated gate icon with translation (representing gate sliding open/closed)
            AnimatedBuilder(
              animation: _controller,
              builder: (context, child) {
                return Transform.translate(
                  offset: Offset(_offsetAnimation.value, 0), // Move the icon horizontally
                  child: Image.asset(
                    'assets/images/gate.png',
                    height: 100, // Sesuaikan ukuran gambar
                  ),
                );
              },
            ),
            SwitchListTile(
              title: Text('Buka/Tutup Gerbang'),
              value: _isGateOpen,
              onChanged: _toggleGate,
            ),
          ],
        ),
      ),
    );
  }
}
