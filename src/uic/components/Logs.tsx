import React, { useEffect, useState } from "react";

export function Logs(_props: {}): JSX.Element {
  const [logs, setLogs] = useState<string[]>([]);
  const [show, setShow] = useState(false);
  useEffect(() => {
    window.addEventListener("AddLog", (e) => {
      if (e instanceof CustomEvent) {
        setLogs((l) => {
          let p;
          if (l.length > 50000) {
            p = l.slice(500, -1);
          } else {
            p = l.concat();
          }
          p.push(String(e.detail));
          return p;
        });
      }
    });
    window.addEventListener("keyup", (e) => {
      if (e.key == "Tab") {
        setShow((s) => !s);
      }
    });
  }, []);
  return (
    <div
      style={{
        position: "fixed",
        left: 0,
        right: 0,
        top: show ? 0 : "200vh",
        transition: "200ms",
        bottom: 0,
        backgroundColor: "#000000cc",
        zIndex: 1024,
        pointerEvents: show ? undefined : "none",
      }}
    >
      <p
        style={{
          fontFamily:
            "'Source Code Pro', 'Fira Code', Monaco, Consolas, 'Courier New', Courier, monospace",
          width: "90%",
          height: "90%",
          overflowY: "scroll",
          margin: "2rem 2rem",
          color: "white",
          lineBreak: "anywhere",
          wordBreak: "break-word",
        }}
      >
        {logs.map((l, i) => {
          const pl = l.split("] ");
          let mt = pl.shift();
          mt += "] ";
          const ms = pl.join("] ");
          return (
            <React.Fragment key={i}>
              <span className={"a2logmt"}>{mt}</span>
              {ms}
              <br />
            </React.Fragment>
          );
        })}
      </p>
      <br />
    </div>
  );
}
