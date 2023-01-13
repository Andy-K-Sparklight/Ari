import React, { useState } from "react";
import { Tag } from "./Tag";

interface TextInputProps {
  tag?: string;
  placeholder?: string;
  value?: string;
  onChange?: (s: string, err: boolean) => void;
  type?: string;
}

const PLAYER_NAME_REG = /^[0-9a-z_]{3,16}$/i;

export function TextInput(props: TextInputProps): JSX.Element {
  const [err, setErr] = useState(true);
  let type = "text";
  if (props.type == "PlayerID") {
  }
  console.log(err);
  return (
    <>
      <Tag s={props.tag} />
      <input
        style={{
          borderColor:
            err && (props.value || "").length > 0 ? "red" : undefined,
        }}
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
