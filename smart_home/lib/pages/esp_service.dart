import 'package:http/http.dart' as http;

class EspService {
  final String baseUrl;

  // Constructor menerima base URL ESP8266
  EspService(this.baseUrl);

  // Fungsi untuk mengirim perintah ke ESP8266
  Future<void> sendCommand(String endpoint) async {
    try {
      final response = await http.get(Uri.parse('$baseUrl/$endpoint'));
      if (response.statusCode == 200) {
        print('Command $endpoint berhasil dikirim');
      } else {
        print('Gagal mengirim perintah: ${response.statusCode}');
      }
    } catch (e) {
      print('Error: $e');
    }
  }
}
