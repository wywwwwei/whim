import React, { useState } from "react";

export default function Five() {
    const [isToggled, setIsToggled] = useState(false)
    const handleToggle = () => {
        setIsToggled(!isToggled)
    }
    return (
        <div>
            <label>
                <input type="checkbox" onChange={handleToggle}></input>
            </label>
            <p>{isToggled ? "On" : "Off"}</p>
        </div>
    )
}