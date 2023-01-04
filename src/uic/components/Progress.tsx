import React, { useState } from "react";
import { CircularProgressbar, buildStyles } from "react-circular-progressbar";
import "react-circular-progressbar/dist/styles.css";

export function Progress(_props: {}): JSX.Element {
  const [progress, setProgress] = useState(0);

  useState(() => {
    const foo = (e: Event) => {
      if (e instanceof CustomEvent) {
        const pst = parseInt(e.detail);
        setProgress(pst);
      }
    };
    window.addEventListener("SetProgress", foo);
    return () => {
      window.removeEventListener("SetProgress", foo);
    };
  });
  return (
    <div style={{ width: "15rem" }}>
      <CircularProgressbar
        value={progress}
        text={Math.floor(progress) + "%"}
        styles={buildStyles({
          pathColor: "var(--a2-base)",
          textColor: "var(--a2-text)",
        })}
      />
    </div>
  );
}
