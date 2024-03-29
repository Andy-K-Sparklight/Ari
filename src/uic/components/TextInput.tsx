import React, { useEffect, useState } from "react";
import { Tag } from "./Tag";

interface TextInputProps {
  tag?: string;
  placeholder?: string;
  value?: string;
  onChange?: (s: string, err: boolean) => void;
  type?: string;
  autoFocus?: boolean;
}

const PLAYER_NAME_REG = /^[0-9a-z_]{3,16}$/i;
const SERVER_URL_REG =
  /^https?:\/\/(?:www\.)?[-a-z0-9@:%._\+~#=]{1,256}\.[a-z0-9()]{1,6}\b(?:[-a-z0-9()@:%_\+.~#?&\/=]*)$/i;

export function TextInput(props: TextInputProps): JSX.Element {
  const [err, setErr] = useState(true);
  let type = "text";
  if (props.type == "password") {
    type = props.type;
  }
  useEffect(() => {
    if (props.type == "YggServer") {
      window.ondragover = (e) => {
        e.preventDefault();
      };
      window.ondrop = (e: DragEvent) => {
        e.preventDefault();
        const pd = e.dataTransfer?.getData("text/plain");
        if (pd?.startsWith("authlib-injector:yggdrasil-server:")) {
          const sv = pd.slice("authlib-injector:yggdrasil-server:".length);
          if (sv) {
            if (props.onChange) {
              setErr(false);
              props.onChange(decodeURIComponent(sv), false);
            }
          }
        }
      };
      return () => {
        window.ondrop = () => {};
      };
    }
  });

  return (
    <>
      <Tag s={props.tag} />
      <input
        style={{
          borderColor:
            err && (props.value || "").length > 0 ? "red" : undefined,
        }}
        autoFocus={props.autoFocus}
        className={"a2input"}
        type={type}
        value={props.value}
        onChange={(s) => {
          let e = false;
          if (props.type == "PlayerID") {
            if (!PLAYER_NAME_REG.test(s.target.value)) {
              e = true;
            }
          }
          if (props.type == "YggServer") {
            if (!SERVER_URL_REG.test(s.target.value)) {
              e = true;
            }
          }
          e = s.target.value.length == 0;
          setErr(e);
          if (props.onChange) {
            props.onChange(s.target.value, e);
          }
        }}
        placeholder={props.placeholder}
      />
    </>
  );
}
