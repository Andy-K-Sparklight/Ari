import React, { useEffect, useRef, useState } from "react";
import { CircularProgressbar, buildStyles } from "react-circular-progressbar";
import "react-circular-progressbar/dist/styles.css";
import { sendMessage } from "../Protocol";
import { tr } from "./TP";

export function Progress(_props: {}): JSX.Element {
  const [progress, setProgress] = useState(-1);
  const [step, setStep] = useState("");
  const [bk, setBreak] = useState(false);
  const mounted = useRef(false);

  useEffect(() => {
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
  useEffect(() => {
    mounted.current = true;
    return () => {
      mounted.current = false;
    };
  });
  useEffect(() => {
    const foo = (e: Event) => {
      if (e instanceof CustomEvent) {
        const pst = e.detail;
        setStep(pst);
      }
    };
    const kl = (e: KeyboardEvent) => {
      if (e.key == "Escape") {
        if (bk) {
          void sendMessage("RequestBreak", "");
        } else {
          setBreak(true);
          setTimeout(() => {
            if (mounted.current) {
              setBreak(false);
            }
          }, 5000) as unknown as number;
        }
      }
    };
    window.addEventListener("SetStep", foo);
    window.addEventListener("keydown", kl);
    return () => {
      window.removeEventListener("SetStep", foo);
      window.removeEventListener("keydown", kl);
    };
  });
  return (
    <div style={{ width: "15rem", overflow: "visible", marginRight: "4vw" }}>
      <CircularProgressbar
        value={progress >= 0 ? progress : 33}
        text={progress >= 0 ? Math.floor(progress) + "%" : ""}
        className={progress < 0 ? "a2progindef" : undefined}
        styles={buildStyles({
          pathColor: "var(--a2-base)",
          textColor: "var(--a2-text)",
        })}
      />
      <div
        style={{
          textAlign: "center",
          color: "var(--a2-base)",
          marginTop: "2vh",
          overflow: "visible",
        }}
        className={".a2submit"} // Borrow this
      >
        <br /> <br />
        {step ? tr("FlowSteps." + step) : ""}
        <span
          style={{
            color: "var(--a2-warn)",
            width: "40vw",
            overflow: "visible",
          }}
        >
          <br />
          <br />
          {bk ? (
            <>
              {tr("Commons.Cancel")
                .split("\n")
                .map((l, i) => {
                  return (
                    <span
                      key={i}
                      dangerouslySetInnerHTML={{ __html: l + "<br/>" }}
                    />
                  );
                })}
            </>
          ) : (
            ""
          )}
        </span>
      </div>
    </div>
  );
}
