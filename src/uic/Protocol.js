const PROTOCOL_CTL = new Map();
let pid = 0;

export function initProtocol() {
  window.tellSize(
    screen.availWidth,
    screen.availHeight,
    window.outerWidth,
    window.outerHeight
  );
  window.a2Call = (chn, msg) => {
    window.dispatchEvent(new CustomEvent(chn, { detail: msg }));
  };
  window.a2Reply = (msg, pid) => {
    const f = PROTOCOL_CTL[pid];
    if (f) {
      f(msg);
    }
  };
}

export function sendMessage(chn, msg) {
  pid++;
  return new Promise((res) => {
    window.SYS(chn, pid, msg);
    PROTOCOL_CTL[pid] = res;
  });
}
