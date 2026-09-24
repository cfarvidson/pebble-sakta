function currentSettings() {
  return { background: localStorage.getItem('background') || 'cream' };
}

function sendSettings(cfg) {
  var code = { cream: 0, white: 1, black: 2 }[cfg.background] || 0;
  Pebble.sendAppMessage(
    { 'BACKGROUND': code },
    function() { console.log('Settings sent: ' + JSON.stringify(cfg)); },
    function(err) { console.log('Failed to send settings: ' + JSON.stringify(err)); }
  );
}

var CONFIG_HTML =
  '<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width">' +
  '<style>body{font-family:-apple-system,sans-serif;margin:20px;background:#f5f5f5;color:#111}' +
  '.card{background:#fff;border-radius:8px;padding:16px;margin-bottom:12px;box-shadow:0 1px 3px rgba(0,0,0,.12)}' +
  '.row{display:flex;align-items:center;padding:8px 0}' +
  'label{font-size:16px;margin-left:10px}input[type=radio]{width:22px;height:22px}' +
  '.swatch{display:inline-block;width:22px;height:22px;border-radius:50%;border:1px solid #999;margin-left:auto}' +
  'button{width:100%;padding:14px;background:#007aff;color:#fff;border:none;border-radius:8px;font-size:16px;margin-top:16px}' +
  '</style></head><body><h2>Sakta</h2>' +
  '<div class="card"><div style="font-size:14px;color:#666;margin-bottom:4px">Dial colour</div>' +
  '<div class="row"><input type="radio" name="bg" id="cream" value="cream"><label for="cream">Cream</label><span class="swatch" style="background:#ffffaa"></span></div>' +
  '<div class="row"><input type="radio" name="bg" id="white" value="white"><label for="white">White</label><span class="swatch" style="background:#fff"></span></div>' +
  '<div class="row"><input type="radio" name="bg" id="black" value="black"><label for="black">Black</label><span class="swatch" style="background:#000"></span></div>' +
  '</div>' +
  '<button onclick="submit()">Save</button>' +
  '<script>' +
  'var o=JSON.parse(decodeURIComponent(location.hash.substring(1))||"{}");' +
  'document.getElementById(o.background||"cream").checked=true;' +
  'function submit(){var r={background:document.querySelector("input[name=bg]:checked").value};' +
  'document.location="pebblejs://close#"+encodeURIComponent(JSON.stringify(r))}' +
  '</script></body></html>';

Pebble.addEventListener('ready', function() {
  sendSettings(currentSettings());
});

Pebble.addEventListener('showConfiguration', function() {
  var hash = encodeURIComponent(JSON.stringify(currentSettings()));
  Pebble.openURL('data:text/html,' + encodeURIComponent(CONFIG_HTML) + '#' + hash);
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (!e || !e.response) return;
  var cfg = JSON.parse(decodeURIComponent(e.response));
  localStorage.setItem('background', cfg.background);
  sendSettings(cfg);
});
