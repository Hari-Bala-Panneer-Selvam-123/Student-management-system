import React, { useState, useEffect } from 'react';
import './App.css';


function App() {
  const [students, setStudents] = useState([]);
  const [formData, setFormData] = useState({
    name: '',
    rollNo: '',
    marks: '',
    grade: '',
  });
  const [searchRollNo, setSearchRollNo] = useState('');
  const [searchResult, setSearchResult] = useState(null);
  const [errorMessage, setErrorMessage] = useState('');

  const backendUrl = 'http://localhost:8080'; 

  // Fetch all students when the component loads
  useEffect(() => {
    fetchStudents();
  }, []);

  const fetchStudents = () => {
    fetch(`${backendUrl}/students`)
      .then(res => {
        if (!res.ok) throw new Error('Failed to fetch students');
        return res.json();
      })
      .then(data => {
        const list = Object.values(data);
        setStudents(list);
        setErrorMessage(''); 
      })
      .catch(err => {
        console.error(err);
        setErrorMessage('Error fetching students. Please check the server.');
      });
  };

  const handleInputChange = (e) => {
    const { name, value } = e.target;
    setFormData(prevState => ({
      ...prevState,
      [name]: value,
    }));
  };

  const addStudent = () => {
    const { name, rollNo, marks, grade } = formData;

    if (!name || !rollNo || !marks || !grade) {
      setErrorMessage('All fields are required.');
      return;
    }

    fetch(`${backendUrl}/students`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        name,
        roll_no: parseInt(rollNo),
        marks: parseInt(marks),
        grade,
      }),
    })
      .then(res => {
        if (!res.ok) throw new Error('Failed to add student');
        fetchStudents();
        setFormData({ name: '', rollNo: '', marks: '', grade: '' });
        setErrorMessage(''); // Clear error message on success
      })
      .catch(err => {
        console.error(err);
        setErrorMessage('Error adding student. Please try again.');
      });
  };

  const updateStudent = () => {
    const { name, rollNo, marks, grade } = formData;

    if (!name || !rollNo || !marks || !grade) {
      setErrorMessage('All fields are required.');
      return;
    }

    fetch(`${backendUrl}/students/${rollNo}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        name,
        roll_no: parseInt(rollNo),
        marks: parseInt(marks),
        grade,
      }),
    })
      .then(res => {
        if (!res.ok) throw new Error('Failed to update student');
        fetchStudents();
        setFormData({ name: '', rollNo: '', marks: '', grade: '' });
        setErrorMessage(''); // Clear error message on success
      })
      .catch(err => {
        console.error(err);
        setErrorMessage('Error updating student. Please try again.');
      });
  };

  const deleteStudent = (rollNo) => {
    fetch(`${backendUrl}/students/${rollNo}`, {
      method: 'DELETE',
    })
      .then(res => {
        if (!res.ok) throw new Error('Failed to delete student');
        fetchStudents();
      })
      .catch(err => {
        console.error(err);
        setErrorMessage('Error deleting student. Please try again.');
      });
  };

  const searchStudent = () => {
    if (!searchRollNo) {
      setErrorMessage('Roll number is required for search.');
      return;
    }

    fetch(`${backendUrl}/students/${searchRollNo}`)
      .then(res => {
        if (!res.ok) throw new Error('Student not found');
        return res.json();
      })
      .then(data => {
        setSearchResult(data);
        setErrorMessage(''); // Clear error message on success
      })
      .catch(err => {
        console.error(err);
        setSearchResult(null);
        setErrorMessage('Student not found. Please try another roll number.');
      });
  };

  return (
    <div style={{ textAlign: 'center', marginTop: '30px' }}>
      <h1>Student Record Management</h1>
      {errorMessage && <p style={{ color: 'red' }}>{errorMessage}</p>} {/* Display error messages */}

      <div style={{ marginBottom: '20px' }}>
        <h2>Add / Update Student</h2>
        <input name="name" placeholder="Name" value={formData.name} onChange={handleInputChange} /><br />
        <input name="rollNo" placeholder="Roll No" value={formData.rollNo} onChange={handleInputChange} /><br />
        <input name="marks" placeholder="Marks" value={formData.marks} onChange={handleInputChange} /><br />
        <input name="grade" placeholder="Grade" value={formData.grade} onChange={handleInputChange} /><br /><br />
        <button onClick={addStudent}>Add Student</button>
        <button onClick={updateStudent} style={{ marginLeft: '10px' }}>Update Student</button>
      </div>

      <div style={{ marginBottom: '20px' }}>
        <h2>Search Student</h2>
        <input placeholder="Roll No to Search" value={searchRollNo} onChange={e => setSearchRollNo(e.target.value)} />
        <button onClick={searchStudent} style={{ marginLeft: '10px' }}>Search</button>
        {searchResult && (
          <div style={{ marginTop: '10px' }}>
            <p><b>Name:</b> {searchResult.name}</p>
            <p><b>Roll No:</b> {searchResult.roll_no}</p>
            <p><b>Marks:</b> {searchResult.marks}</p>
            <p><b>Grade:</b> {searchResult.grade}</p>
          </div>
        )}
      </div>

      <div>
        <h2>All Students</h2>
        <ul style={{ listStyleType: 'none', padding: 0 }}>
  {students.map((student, index) => (
    <li key={index} className="student-item">
      <span>
        <b>{student.name}</b> (Roll No: {student.roll_no}) - {student.marks} Marks [{student.grade}]
      </span>
      <button className="delete" onClick={() => deleteStudent(student.roll_no)}>Delete</button>
    </li>
  ))}
</ul>

      </div>
    </div>
  );
}

export default App;
