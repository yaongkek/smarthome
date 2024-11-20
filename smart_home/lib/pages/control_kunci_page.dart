import 'package:flutter/material.dart';

class ControlKunciPage extends StatefulWidget {
  @override
  _ControlKunciPageState createState() => _ControlKunciPageState();
}

class _ControlKunciPageState extends State<ControlKunciPage> with TickerProviderStateMixin {
  bool _isLocked = true;
  late AnimationController _controller;
  late Animation<double> _offsetAnimation;

  @override
  void initState() {
    super.initState();
    // Initialize the animation controller for lock/unlock animation
    _controller = AnimationController(
      duration: Duration(seconds: 1),
      vsync: this,
    );

    // Define the offset animation (horizontal translation for gate opening)
    _offsetAnimation = Tween<double>(begin: 0, end: 50).animate(_controller);
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  void _toggleLock(bool value) {
    setState(() {
      _isLocked = value;
      if (_isLocked) {
        _controller.reverse(); // Close the gate
      } else {
        _controller.forward(); // Open the gate
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Control Kunci Rumah'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            // Animated gate icon with translation (representing gate opening)
            AnimatedBuilder(
              animation: _controller,
              builder: (context, child) {
                return Transform.translate(
                  offset: Offset(_offsetAnimation.value, 0), // Slide the icon horizontally
                  child: Icon(
                    _isLocked ? Icons.lock : Icons.lock_open, // Change icon based on lock status
                    size: 100,
                    color: Colors.blue,
                  ),
                );
              },
            ),
            SwitchListTile(
              title: Text('Kunci/Buka Rumah'),
              value: _isLocked,
              onChanged: _toggleLock,
            ),
          ],
        ),
      ),
    );
  }
}
