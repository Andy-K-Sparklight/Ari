import React, { useState } from "react";
import { CircularProgressbar, buildStyles } from "react-circular-progressbar";
import "react-circular-progressbar/dist/styles.css";

export function Progress(_props: {}): JSX.Element {
  const [progress, setProgress] = useState(-1);

  useState(() => {
    const foo = (e: Event) => {
      if (e instanceof CustomEvent) {
        const pst = parseFloat(e.detail) || 0;
        setProgress(Math.floor(pst * 100) / 100);
      }
    };
    window.addEventListener("SetProgress", foo);
    return () => {
      window.removeEventListener("SetProgress", foo);
    };
  });
  return (
    <div style={{ width: "15rem", overflow: "hidden", marginRight: "4vw" }}>
      <CircularProgressbar
        value={progress >= 0 ? progress : 33}
        text={progress >= 0 ? Math.floor(progress) + "%" : ""}
        className={progress < 0 ? "a2progindef" : undefined}
        styles={buildStyles({
          pathColor: "var(--a2-base)",
          textColor: "var(--a2-text)",
        })}
      />
    </div>
  );
}
