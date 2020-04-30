mosquitto_sub -h eu.thethings.network -t '+/devices/+/events/activations' -u 'deepsleep' -P 'ttn-account-v2.NeOz09IzSQr8GbB8SNq87VhqPayNn91jl_oB4oYjVeI' -v
mosquitto_sub -h eu.thethings.network -t '+/devices/+/up' -u 'deepsleep' -P 'ttn-account-v2.NeOz09IzSQr8GbB8SNq87VhqPayNn91jl_oB4oYjVeI' -v


function Decoder(bytes, port) {
    var decoded = {};
    decoded.temps = ((bytes[0] << 8) + bytes[1]) / 100;
    decoded.onewire = (bytes[2]) ;
    decoded.bat =  ((bytes[3] << 8) | bytes[4]) /100;
    decoded.bootcount =  ((bytes[5] << 8) | bytes[6]);
    return decoded;
}
