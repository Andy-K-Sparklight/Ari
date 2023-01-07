import React, { useEffect, useState } from "react";

function humanSize(bytes: number) {
  const thresh = 1024;

  if (Math.abs(bytes) < thresh) {
    return bytes + " B";
  }

  const units = ["KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"];
  let u = -1;
  const r = 10;

  do {
    bytes /= thresh;
    ++u;
  } while (
    Math.round(Math.abs(bytes) * r) / r >= thresh &&
    u < units.length - 1
  );

  return bytes.toFixed(1) + " " + units[u];
}
export function NetSpeed(_props: {}): JSX.Element {
  const [speed, setSpeed] = useState(0);
  useEffect(() => {
    window.addEventListener("SetNetSpeed", (e) => {
      if (e instanceof CustomEvent) {
        setSpeed(parseInt(e.detail));
      }
    });
  }, []);

  return (
    <div
      style={{
        position: "absolute",
        right: "1vw",
        bottom: "2px",
        color: "var(--a2-base)",
      }}
    >
      {humanSize(speed) + "/s"}
    </div>
  );
}
