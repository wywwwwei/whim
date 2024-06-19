import React, { useState } from 'react'

export default function Nine() {
    const [backgroundColor, setBackgroundColor] = useState('white')

    const handleClick = () => {
        setBackgroundColor(backgroundColor === 'white' ? 'lightblue' : 'white')
    }

    return (
        <div onClick={handleClick} style={{
            backgroundColor: backgroundColor,
            widows:'2000px',
            height:'200px',
            cursor:'pointer'
        }}>
            Click me to change color
        </div>
    )
}