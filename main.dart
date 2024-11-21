import 'package:flutter/material.dart';
import 'package:smart_home/auth/login_page.dart';
import 'package:smart_home/auth/register_page.dart';
import 'splash_screen_page.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'IoT Home',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home:
          SplashScreenPage(), // Menampilkan SplashScreenPage sebagai halaman utama
      routes: {
        '/login': (context) => LoginPage(),
        '/register': (context) => RegisterPage(),
        '/home': (context) => HomePage(),
      },
    );
  }
}
//ghdtdjtfffyif