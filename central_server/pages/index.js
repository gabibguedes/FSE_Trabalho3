import Head from 'next/head'
import Image from 'next/image'
import styles from '../styles/Home.module.css'
import Mqtt from '../components/mqtt'

export default function Home() {
  return (
    <div className={styles.container}>
      <Head>
        <title>Central server</title>
        <link rel="icon" href="/favicon.ico" />
      </Head>

      <div>
        <Mqtt/>
      </div>
    </div>
  )
}
