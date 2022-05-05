import fs from 'fs'

const LOG_FILE = process.env.NEXT_PUBLIC_LOG_FILE

export default function log(req, res) {
  const {
    id, room, modeName, action, value
  } = req.body
  const date = new Date().toString()
  const content = `${date}, ${id}, ${room}, ${modeName}, ${action}, ${value}\n`
  fs.writeFile(LOG_FILE, content, { flag: 'a+' }, () => {})
  res.status(200).send()
}
