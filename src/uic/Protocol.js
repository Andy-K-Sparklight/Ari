const PROTOCOL_CTL = new Map();
let pid = 0;

export async function initProtocol() {
  window.a2Call = (chn, msg) => {
    window.dispatchEvent(new CustomEvent(chn, { detail: msg }));
  };
  window.a2Reply = (msg, pid) => {
    const f = PROTOCOL_CTL[pid];
    if (f) {
      f(msg);
    }
  };
  // Wait for window.SYS
  return new Promise((res) => {
    const p = setInterval(() => {
      if (window.SYS) {
        res();
        clearInterval(p);
      }
    }, 10);
  });
}

export function sendMessage(chn, msg) {
  pid++;
  return new Promise((res) => {
    window.SYS(chn, pid, msg);
    PROTOCOL_CTL[pid] = res;
  });
}
